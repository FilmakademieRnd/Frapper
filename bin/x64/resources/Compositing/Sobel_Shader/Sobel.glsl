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
**                                  sobel_ps								 **
**                                                                           **
*******************************************************************************/
#version 400 core

const mat3 G2[2] = mat3[](
	mat3(1.0, 2.0, 1.0, 0.0, 0.0, 0.0, -1.0, -2.0, -1.0),
	mat3(1.0, 0.0, -1.0, 2.0, 0.0, -2.0, 1.0, 0.0, -1.0)
);


out vec4 oColor;
in vec2 oTexCoord;
uniform sampler2D inputMap;

void main()
{
	mat3 I;
	float cnv[2];
	vec3 samples = vec3(1,0,0);

	/* fetch the 3x3 neighbourhood and use the RGB vector's length as intensity value */
	for (int i = 0; i<3; i++)
		for (int j = 0; j<3; j++) {
			samples = texelFetch(inputMap, ivec2(gl_FragCoord) + ivec2(i - 1, j - 1), 0).rgb;
			I[i][j] = length(samples);
		}

	/* calculate the convolution values for all the masks */
	for (int i = 0; i<2; i++) {
		float dp3 = dot(G2[i][0], I[0]) + dot(G2[i][1], I[1]) + dot(G2[i][2], I[2]);
		cnv[i] = dp3 * dp3;
	}

	oColor = vec4(vec3(0.5 * sqrt(cnv[0] * cnv[0] + cnv[1] * cnv[1])), 1.0);
}