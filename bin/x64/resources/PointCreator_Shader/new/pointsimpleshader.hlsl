/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2014 Filmakademie Baden-Wuerttemberg, Institute of Animation 

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; version 2.1 of the License.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
-----------------------------------------------------------------------------
*/

Texture2D <float4> RTT;

static const float ViewportRes = 512;
static const uint pointPerPrimitive = 64;

struct A2V {
	float4 pos	:POSITION;
};

struct V2H {
	float4 pos	:POSITION;
};

struct V2G {
	float4 pos	:POSITION;
};

struct HCO {
	float Edges[2]	:SV_TessFactor;
};

struct H2D {
	float4 pos	:POSITION;
};

struct D2G {
	float4 pos	:POSITION;
};

struct G2P {
	float4 pos	:SV_POSITION;
};

struct P2F{
	float4 col	:SV_TARGET;
};

//VERTEX SHADER
/////////////////////////////////////////////////////

V2G main_vs( 	in A2V In,
				uniform float4x4 wvpMat)
{
	
	V2G Out;
	
    Out.pos = mul(wvpMat, In.pos);
	
	return Out;
}

//HULL SHADER
/////////////////////////////////////////////////////

//patch constant function
HCO patchConstantFunction()	{
	
	HCO Out;
	
	Out.Edges[0] = 1.0f; //detail = line copy
	Out.Edges[1] = 2.0f; //density = divisions
	
	return Out;
}

//tesselation variables
[domain("isoline")]
[partitioning("integer")]
[outputtopology("line")]
[outputcontrolpoints(1)]
[patchconstantfunc("patchConstantFunction")]
// hull main function
H2D main_hs(	InputPatch<V2H,1> In,
				uint Id :SV_PrimitiveID	)	{
	
	H2D Out;
	
	Out.pos = In[0].pos;

	return Out;
}

//DOMAIN SHADER
/////////////////////////////////////////////////////

//tesselation variables
[domain("isoline")]
D2G main_ds(	HCO In,
				OutputPatch<H2D, 1> outPatch,
				float2 uv : SV_DomainLocation	){
				
	D2G Out;
	
	Out.pos = outPatch[0].pos;
	
	return Out;
}

//GEOMETRY SHADER
/////////////////////////////////////////////////////

void create_square(	float3 position,
					float3 upVec,
					float3 rightVec,
					uniform float4x4 wvpMat,
					inout TriangleStream<G2P> Out	) {
	
	G2P vA,vB,vC,vD;
	
	vA.pos = mul(wvpMat,float4( upVec+rightVec+position,1));
	vB.pos = mul(wvpMat,float4(-upVec+rightVec+position,1));
	vC.pos = mul(wvpMat,float4(-upVec-rightVec+position,1));
	vD.pos = mul(wvpMat,float4( upVec-rightVec+position,1));
	Out.Append(vA);
	Out.Append(vB);
	Out.Append(vD);
	Out.Append(vC);				
	
}

[maxvertexcount(256)]
void main_gs(	point V2G In[1],
				uint pID	:SV_PrimitiveID,
				inout TriangleStream<G2P> Out,
				uniform float4x4 wvpInvMat,
				uniform float4x4 wvpMat,
				uniform float4x4 vMat,
				uniform float4 viewSize,
				uniform float radius	) {
	
	// for all points
	float pixelSize = radius*In[0].pos.w*.001;
	float4x4 TvMat = transpose(vMat);
	float4 invPos = mul(wvpInvMat,In[0].pos);
	float3 camUp = normalize(float3(TvMat._12,TvMat._22,TvMat._32))*(pixelSize);
	float3 camRight = normalize(float3(TvMat._11,TvMat._21,TvMat._31))*(pixelSize);
				
	bool done = 0;
	int i=0;
	//while(!done&&i<pointPerPrimitive){
		
		int x = (i+pointPerPrimitive*pID)%ViewportRes;
		int y = (i+pointPerPrimitive*pID)/ViewportRes;
	
		float4 samplePos = RTT.Load(int3(x,y,0));
		
		/*
		if(!samplePos.w) {
			done=1;
			break;
		}
		
		if(x==ViewportRes&&y==ViewportRes){
			done = 1;
			break;
		}
		*/
		
		float4 targetPos=float4(pID*.25,i*.25,0,0);
		
		create_square(targetPos.xyz,camUp,camRight,wvpMat,Out);
		
		Out.RestartStrip();
		
		i++;
	//}
}

//PIXEL SHADER
/////////////////////////////////////////////////////

void main_ps(in G2P In,
             out P2F Out)
{
    Out.col = float4(1.0,0.0,0.0,1.0);
}