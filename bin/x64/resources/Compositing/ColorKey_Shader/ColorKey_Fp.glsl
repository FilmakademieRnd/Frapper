/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2016 Filmakademie Baden-Wuerttemberg, Institute of Animation 

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

/////////////////////
// ColorKey-Shader //
/////////////////////

/////////////////
// Key-Pass //
/////////////////

in  vec2 oTexCoord;
out vec4 oColor;
uniform vec3 key;
uniform float epsilon;
uniform sampler2D sceneTex;

void main()
{
	vec3 inColor = vec3(texture2D(sceneTex, oTexCoord).rgb);
	float alpha = 1.0;
	vec3 delta = vec3(abs(inColor - key));
	if (delta.r < epsilon &&
		delta.g < epsilon && 
		delta.b < epsilon)
		alpha = 0.0;
	//else
		//alpha = (delta.r + delta.g + delta.b) / 3.0f;  for color blending
	
	oColor = vec4(alpha);
}