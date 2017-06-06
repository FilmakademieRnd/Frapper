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

/**************************************************************************************
**                                                                                   **
**                                    ssao_Quad_vs                                   **
**                                                                                   **
***************************************************************************************/
#version 400 core


// I/O
in vec4 vertex;
out vec2 vsUv0;
out vec3 vsRay;
out vec2 vsRandUV;
uniform vec3 farCorner;
uniform vec2 textureSize;
uniform vec2 randomMapSize;
uniform mat4 worldViewProj;

void main()
{
	// Use standardise transform, so work accord with render system specific (RS depth, requires texture flipping, etc)
	gl_Position = worldViewProj * vertex;

	// The input positions adjusted by texel offsets, so clean up inaccuracies
	vec2 sVertex = sign(vertex.xy);

	// Convert to image-space
	vsUv0 = (vec2(sVertex.x, -sVertex.y) + 1.0) * 0.5; //iTexCoord; würde auch funktionieren

	// divide by the size of random map
	float factX = textureSize.x / randomMapSize.x;
	float factY = textureSize.y / randomMapSize.y;

	vsRandUV = vec2(vsUv0.x * factX, vsUv0.y * factY);

	// calculate the correct ray (modify XY parameters based on screen-space quad XY)
	vsRay = farCorner * vec3(sign(sVertex), 1.0);
}