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
	float4 col	:SV_TARGET;
};

/////////////////////////////////////////////////////

void main_vs( 	in A2V In,
				out V2G Out,
				uniform float4x4 wvpMat)
{
    Out.pos = mul(wvpMat, In.pos);
}

/////////////////////////////////////////////////////

[maxvertexcount(10)]
void main_gs(	point V2G In[1],
				inout TriangleStream<G2P> Out,
				uniform float4x4 wvpInvMat,
				uniform float4x4 wvpMat) {

	float4 invPos = mul(wvpInvMat,In[0].pos);
	
	G2P vA,vB,vC,vD;
	
	vA.pos = mul(wvpMat,float4( 1, 1,0,1)+invPos);
	vB.pos = mul(wvpMat,float4( 1,-1,0,1)+invPos);
	vC.pos = mul(wvpMat,float4(-1,-1,0,1)+invPos);
	vD.pos = mul(wvpMat,float4(-1, 1,0,1)+invPos);
	Out.Append(vA);
	Out.Append(vB);
	Out.Append(vC);
	Out.Append(vC);
	Out.Append(vD);
	Out.Append(vA);

}
/////////////////////////////////////////////////////

void main_ps(in G2P In,
             out P2F Out)
{
    Out.col = float4(1.0,0.0,0.0,1.0);
}