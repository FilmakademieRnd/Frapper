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

/////////////////////////////////////////////////
///////////inputs

/////////////////////////////////////////////////////
/////structs

struct A2V {
	float4 pos	:POSITION;
};

struct V2G {
	float4 pos	:POSITION;
};

struct G2P {
	float4 pos	:SV_POSITION;
};

struct P2F{
	float4 col	:SV_TARGET;
};

static  float ViewportRes = 512;

cbuffer inputBuffer : register(b[0]) {
	int pass;
}
/////////////////////////////////////////////////////

void main_vs( 	in A2V In,
				out V2G Out	)
{
    Out.pos = In.pos;
}

/////////////////////////////////////////////////////
// creates a flat rectangle over the viewport, no need to change for flatshaders
// also here we calculate the random value array
[maxvertexcount(4)]
void main_gs(	line V2G In[2],
				inout TriangleStream<G2P> Out) {
	
	G2P vA,vB,vC,vD;

	vA.pos = float4( 1, 1, 0, 0);
	vB.pos = float4( 1,-1, 0, 0);
	vC.pos = float4(-1,-1, 0, 0);
	vD.pos = float4(-1, 1, 0, 0);
	Out.Append(vA);
	Out.Append(vB);
	Out.Append(vC);
	Out.Append(vD);
}
/////////////////////////////////////////////////////

void main_ps(	in G2P In,
				out P2F Out	)
{
	
	// set the output
	if(((In.pos.x-.5f)+(In.pos.y-.5f)*ViewportRes)<count){
		float2 p = (float2)(In.pos.xy)/ViewportRes;
		float n = 0;
		if(process<=2) {
			n = fBm(p);
			if(process==1) n = abs(n);
			else if(process==2) n = (n+1)*.5;
		}
		else if(process==3) n = turbulence(p);
		else if(process==4) n = ridgedmf(p);
		
		if(n<=1)
			Out.col = float4(n,n,n,1);
		else Out.col = float4(1,0,0,1);
	} else {
		Out.col = float4(0,0,0,0);
	}
	
	
	
}
