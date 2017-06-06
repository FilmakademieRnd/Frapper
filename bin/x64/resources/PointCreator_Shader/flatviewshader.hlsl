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
	float4 col	:SV_TARGET0;
};

cbuffer inputBuffer : register(b[0]) {
	int count;
	int resolution;
}

/////////////////////////////////////////////////////

void main_vs( 	in A2V In,
				out V2G Out	)
{
    Out.pos = In.pos;
}

/////////////////////////////////////////////////////
// creates a flat rectangle over the viewport, no need to change for flatshaders
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
	if(((In.pos.x-.5f)+(In.pos.y-.5f)*resolution)<count){
		Out.col = float4(0,0,0,1);
	} else {
		Out.col = float4(0,0,0,0);
	}

}