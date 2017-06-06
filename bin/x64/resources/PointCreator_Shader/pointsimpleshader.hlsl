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

static const uint pointPerPrimitive = 64;

cbuffer inputBuffer : register(b[0]){
	int count;
	int resolution;
	float4x4 wvpMat;
	float4x4 wvpInvMat;
	float4x4 vMat;
	float4 viewSize;
	float radius;
}


struct A2V {
	float3 pos	:POSITION;
};

//VERTEX SHADER
/////////////////////////////////////////////////////

struct V2H {
	float3 pos	:POSITION;
};

V2H main_vs( 	in A2V In )
{
	
	V2H Out;
	
	Out.pos = In.pos;
	
	return Out;
}

//HULL SHADER
/////////////////////////////////////////////////////


struct HCO {
	float Edges[2]	:SV_TessFactor;
	//float Inside	:SV_InsideTessFactor;
};

//patch constant function
HCO patchConstantFunction(	InputPatch<V2H,2> inputPatch,
							uint patchId : SV_PrimitiveID	)	{
	
	HCO Out;
	
	/*
	if(count<4096){
		int base = floor(sqrt(count)); //64 => 8
		float leftOver = count-(base*base); //64-64=0
		int dif = ceil(leftOver/base); //0
		
		Out.Edges[0] = 8;//base;// 8 //copy
		Out.Edges[1] = 8;//base+dif; 8 //density = divisions
		*/
	//} else {
		Out.Edges[0] = 64; //copies = actual count
		Out.Edges[1] = 64; //division -> +1 (65)
	//}
	//Out.Edges[2] = 16.0f; //density = divisions
	
	//Out.Inside = 16.0f;
	
	return Out;
}

struct H2D {
	float3 pos	:POSITION;
};

//tesselation variables
[domain("isoline")]
[partitioning("integer")]
[outputtopology("point")]
[outputcontrolpoints(2)]
[patchconstantfunc("patchConstantFunction")]
H2D main_hs(	InputPatch<V2H,2> In,
				uint pointID 	:SV_OutputControlPointID,
				uint PrimID 	:SV_PrimitiveID)	{
	
	H2D Out;
	
	Out.pos = In[pointID].pos;

	return Out;
}

//DOMAIN SHADER
/////////////////////////////////////////////////////

struct D2G {
	float4 pos	:SV_Position;
};

//tesselation variables
[domain("isoline")]
D2G main_ds(	HCO In,
				OutputPatch<H2D, 2> outPatch,
				float2 uv 		:SV_DomainLocation,
				uint PatchID 	:SV_PrimitiveID,
				float tessFactors[2] :SV_TessFactor				)	{
				
	D2G Out;
	
	
	float3 position = 	uv.x * outPatch[0].pos.xyz * tessFactors[1] + //get 64 * 65 static grids
						uv.y * outPatch[1].pos.xyz * tessFactors[0];/* +
						uvw.z * outPatch[2].pos;*/
						
	
	
	//float4 position = 	outPatch[0].pos;
	
	//Out.pos = mul(wvpMat, float4(position,1));
	Out.pos = float4(position,1);
	
	return Out;
}


//GEOMETRY SHADER
/////////////////////////////////////////////////////

struct G2P {
	float4 pos	:SV_Position;
};

void create_square(	float3 position,
					float3 upVec,
					float3 rightVec,
					inout TriangleStream<G2P> Out,
					float4x4 mat) {
	
	G2P vA,vB,vC,vD;
	
	float4 pos4 = mul(mat,float4(position*.1,1));
	upVec = mul(mat,upVec*pos4.w).xyz;
	rightVec = mul(mat,rightVec*pos4.w).xyz;
	
	vA.pos = pos4+float4( upVec+rightVec,0);
	vB.pos = pos4+float4(-upVec+rightVec,0);
	vC.pos = pos4+float4(-upVec-rightVec,0);
	vD.pos = pos4+float4( upVec-rightVec,0);
	
	Out.Append(vA);
	Out.Append(vB);
	Out.Append(vD);
	Out.Append(vC);				
	
}
//[instance(32)]
[maxvertexcount(256)]
void main_gs(	point D2G In[1],
				uint pID	:SV_PrimitiveID,
				inout TriangleStream<G2P> Out			){ //,
				//uint instID	:SV_GSInstanceID	) {
	
	//check if need this instance
	//if((uint)count<(instID)*266240) return; //if counted points have already been assigned
	
	float xID = In[0].pos.x;
	float yID = In[0].pos.y;
	
	float pointID = (xID*65+yID)+(pID*266240);
	if(pointID>count-1) return; //check if we need more from this point
	
	//set radius
	float pixelSize = radius*.0002;
	
	//get view matrix to calculate offset
	float4x4 TvMat = transpose(vMat);
	float3 camUp = normalize(float3(TvMat._12,TvMat._22,TvMat._32))*(pixelSize);
	float3 camRight = normalize(float3(TvMat._11,TvMat._21,TvMat._31))*(pixelSize);
	
	//get number of block Plane sections (block thickness)
	//the number of xy planes needed for the z block size of the current block
	uint sectionCount = (uint)ceil((float)count/4160.0);

	uint i=0;
	while(i<sectionCount&&pointID<count)	{ // for each section of the block
		
		// sampling of texture
		
		int x = floor(pointID%resolution);
		int y = floor(pointID/resolution);
	
		float4 samplePos = RTT.Load(int3(x,y,0));
		
		
		if(samplePos.w) {
		
			float3 targetPos = float3(	In[0].pos.x+64*pID, 	// x = tesselator x count, %4096 per input + instance id (64*64*64 block) count, %262144 per input point
										In[0].pos.y,				// y = tesselator y count, %64 per input
										i);						// z = gs genrated data, %64 per y counter. 

			
			create_square(targetPos,camUp,camRight,Out,wvpMat);
			
			Out.RestartStrip();
		}
		
		pointID+=4160;
		i++;
		
	}
}

//PIXEL SHADER
/////////////////////////////////////////////////////

struct P2F{
	float4 col	:SV_Target;
};

P2F main_ps(in G2P In	) {

	P2F Out;

    Out.col = float4(1.0,0.0,0.0,1.0);
	
	return Out;
}