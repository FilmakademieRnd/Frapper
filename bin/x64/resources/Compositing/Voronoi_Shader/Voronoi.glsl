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

/******************************************************************************
**                                                                           **
**                                  voronoi_ps								 **
**                                                                           **
*******************************************************************************/
#version 400 core

vec2 rhash(vec2 uv) {
	const mat2 t = mat2(.12121212,.13131313,-.13131313,.12121212);
	const vec2 s = vec2(1e4, 1e6);
	uv *= t;
	uv *= s;
	return  fract(fract(uv/s)*uv);
}

float voronoi(in vec2 uv)
{
	vec2 p = floor(uv);
	vec2 f = fract(uv);
	float v = 0.;
	for( int j=-1; j<=1; j++ )
		for( int i=-1; i<=1; i++ )
		{
			vec2 b = vec2(i, j);
			vec2 r = b - f + rhash(p + b);
			v += 1./pow(dot(r,r),8.);
		}
		return pow(1./v, 0.0625);
}

 
out vec4 oColor;
in vec2 oTexCoord;
uniform sampler2D inputMap;
uniform sampler2D lutMap;

void main()
{
	oColor = texture(inputMap, vec2(voronoi(oTexCoord)));

	// 4 debug
	//oColor = float4(tex2D(lutMap, texCoord).rgb, 1.0);
}