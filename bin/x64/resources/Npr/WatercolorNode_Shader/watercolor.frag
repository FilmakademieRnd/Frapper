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
uniform sampler2D granulationMap;
uniform sampler2D noiseMap;
uniform sampler2D paperMap;
uniform vec4 pixelSize;
uniform float noiseScale;
uniform float noiseIntensity;
uniform float paperScale;
uniform float paperIntensity;
uniform float granulationScale;
uniform float granulationIntensity;
uniform float edgeDarkening;
uniform float opacity;
uniform float saturation;


float computeGradient(sampler2D map, vec2 texCoord)
{
	float a00 = texture2D(map, texCoord + vec2(-1.0 * pixelSize.x, -1.0 * pixelSize.y)).a;
	float a01 = texture2D(map, texCoord + vec2(-1.0 * pixelSize.x,  0.0 * pixelSize.y)).a;
	float a02 = texture2D(map, texCoord + vec2(-1.0 * pixelSize.x, +1.0 * pixelSize.y)).a;
	float a10 = texture2D(map, texCoord + vec2( 0.0 * pixelSize.x, -1.0 * pixelSize.y)).a;
	float a11 = texture2D(map, texCoord + vec2( 0.0 * pixelSize.x,  0.0 * pixelSize.y)).a;
	float a12 = texture2D(map, texCoord + vec2( 0.0 * pixelSize.x, +1.0 * pixelSize.y)).a;
	float a20 = texture2D(map, texCoord + vec2(+1.0 * pixelSize.x, -1.0 * pixelSize.y)).a;
	float a21 = texture2D(map, texCoord + vec2(+1.0 * pixelSize.x,  0.0 * pixelSize.y)).a;
	float a22 = texture2D(map, texCoord + vec2(+1.0 * pixelSize.x, +1.0 * pixelSize.y)).a;

	//sobel		
	vec2 gradient = vec2(-a00 - 2.0 * a01 - a02 + a20 + 2.0 * a21 + a22, -a00 - 2.0 * a10 - a20 + a02 + 2.0 * a12 + a22);
	return length(gradient);
		
	////laplacian
	//return -(a00 + a01 + a02 + a10 + a12 + a20 + a21 + a22) + 8.0 * a11;
}


void main (void)
{
	vec2 texCoord = vertexScreenSpace.xy / vertexScreenSpace.w * 0.5 + vec2(0.5);

	float granulation = texture2D(granulationMap, texCoord.xy * granulationScale + pixelSize.zw).r;
	
	
	vec3 noise = texture2D(noiseMap, texCoord.xy * noiseScale).rgb;
	vec2 noiseVec = vec2(noise.x - 0.5, noise.y - 0.5) * noise.z * noiseIntensity;
	
	
	float paper = texture2D(paperMap, texCoord.xy * paperScale).r - 0.5;	
		
		
	vec4 density = texture2D(inputMap, texCoord.xy + noiseVec * pixelSize.xy).rgba;

	
	
	fragColor.a = opacity * density.a;
	
	float darkening = computeGradient(inputMap, texCoord.xy + noiseVec * pixelSize.xy);	
	fragColor.a += edgeDarkening * darkening * density.a;
		
	fragColor.a += granulationIntensity * granulation * density.a;	
	
	fragColor.a += paperIntensity * paper * density.a;

	
	vec3 color = density.rgb;
	
	//color is extra saturated at areas with high density... makes the color feeling more convincing
	vec3 colorSaturated = color + saturation * fragColor.a * (color - vec3( (color.r + color.g + color.b) / 3.0));	
	
	fragColor.rgb = colorSaturated;
	//fragColor.rgb = vec3(1.0, 0.0, 0.0);
}
