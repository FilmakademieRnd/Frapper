/*
-----------------------------------------------------------------------------
This source file is part of the “aQtree” cooperation project between the Institute of Animation, Brainpets GbR and University of Konstanz.
Copyright (c) 2008-2009 Filmakademie Baden-Wuerttemberg, Institute of Animation & University of Konstanz and brainpets GbR

www.aQtree.de

aQtree nodes function in FRAPPER, the framework application developed at the Institute of Animation.
FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper
Copyright (c) 2008-2009 Filmakademie Baden-Wuerttemberg, Institute of Animation

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; version 2.1 of the License.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
-----------------------------------------------------------------------------
*/

#version 400 core


in vec4 vertexScreenSpace;

out vec4 fragColor;


uniform sampler2D inputMap;
uniform vec4 pixelSize;
uniform float threshold;
uniform int monochrome;


void main (void)
{
	vec2 texCoord = vertexScreenSpace.xy / vertexScreenSpace.w * 0.5 + vec2(0.5);

	vec3 a00 = texture2D(inputMap, texCoord + vec2(-1.0 * pixelSize.x, -1.0 * pixelSize.y)).rgb;
	vec3 a01 = texture2D(inputMap, texCoord + vec2(-1.0 * pixelSize.x,  0.0 * pixelSize.y)).rgb;
	vec3 a02 = texture2D(inputMap, texCoord + vec2(-1.0 * pixelSize.x, +1.0 * pixelSize.y)).rgb;
	vec3 a10 = texture2D(inputMap, texCoord + vec2( 0.0 * pixelSize.x, -1.0 * pixelSize.y)).rgb;
	//vec3 a11 = texture2D(inputMap, texCoord + vec2( 0.0 * pixelSize.x,  0.0 * pixelSize.y)).rgb;
	vec3 a12 = texture2D(inputMap, texCoord + vec2( 0.0 * pixelSize.x, +1.0 * pixelSize.y)).rgb;
	vec3 a20 = texture2D(inputMap, texCoord + vec2(+1.0 * pixelSize.x, -1.0 * pixelSize.y)).rgb;
	vec3 a21 = texture2D(inputMap, texCoord + vec2(+1.0 * pixelSize.x,  0.0 * pixelSize.y)).rgb;
	vec3 a22 = texture2D(inputMap, texCoord + vec2(+1.0 * pixelSize.x, +1.0 * pixelSize.y)).rgb;

	//sobel		
	vec2 gradient = vec2(-length(a00) - 2.0 * length(a01) - length(a02) + length(a20) + 2.0 * length(a21) + length(a22), 
	                     -length(a00) - 2.0 * length(a10) - length(a20) + length(a02) + 2.0 * length(a12) + length(a22));
	
	float length = length(gradient);
	if (monochrome == 1) {
		if (length > threshold)
			fragColor.rgba = vec4(0.0, 0.0, 0.0, 1.0);
		else
			fragColor.rgba = vec4(1.0, 1.0, 1.0, 1.0);
	}
	else
		if (length > threshold)
			fragColor.rgba = vec4(gradient, 0.5, 1.0);
		else
			fragColor.rgba = vec4(0.0, 0.0, 0.0, 1.0);

}