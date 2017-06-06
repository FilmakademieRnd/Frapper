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

#version 400 core

//in  vec2 texCoord; 			//: TEXCOORD0,
in  vec4 vertexPosition;
out vec4 fragColor;    		//: COLOR,
uniform float     param0;
uniform float     param1;
uniform float     param2;
uniform float     param3;
uniform float     param4;
uniform sampler2D scene;     //: TEXUNIT0, // prerendered scene (nx,ny,nz,1.0)
uniform sampler2D normal;    //: TEXUNIT1, // normal map in camera space (nx,ny,nz,1.0)
uniform sampler2D depth;     //: TEXUNIT2) // view map (vx,vy,vz) in camera space computed with (vertex-camPosition)

		   
void main() 
{
   vec2 texCoord = vertexPosition.xy / vertexPosition.w * 0.5 + vec2(0.5);
   vec3 n = texture2D(depth, texCoord).xyz;

   if(n.x == 0.0 && n.y == 0.0 && n.z == 0.0) {
     // nothing to do... 
     fragColor = vec4(0.0);
   }

   else {
	   // ...
	   fragColor = vec4(param0, 0.0, 0.0, 1.0);
   }
}
