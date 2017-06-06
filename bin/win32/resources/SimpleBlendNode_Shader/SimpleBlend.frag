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

in vec4 vertexPosition;
out vec4 fragColor;

uniform sampler2D inputMap0;
uniform sampler2D inputMap1;
uniform float op;
uniform float blendFactor;

void main (void)
{

	vec2 texCoord = vertexPosition.xy / vertexPosition.w * 0.5 + vec2(0.5);

	vec4 colorSample0 = texture2D(inputMap0, texCoord);
	vec4 colorSample1 = texture2D(inputMap1, texCoord);
	fragColor = vec4(1.0, 1.0, 1.0, 1.0);

	if (op == 0) {
		//Add
		fragColor.rgb = mix(colorSample0.rgb, colorSample1.rgb, blendFactor);
	}
	else if (op == 1) {
		//Mul
		vec3 color1 = colorSample0.rgb;
		vec3 color2 = colorSample0.rgb * colorSample1.rgb;
		fragColor.rgb = mix(color1, color2, blendFactor);
		fragColor.a = 1.0;
	}
	else if (op == 2) {
		//A over B
        vec4 src = colorSample0;
        vec4 dst = colorSample1;
        float alpha = src.a*blendFactor;
        fragColor = alpha*src + (1.0-alpha)*dst;
	}
	else if (op == 3) {
		//Stencil
		vec3 color2 = colorSample1.rgb;
		float alpha1 = colorSample0.a;
		fragColor.rgb =  color2 * alpha1;
		fragColor.a = alpha1;
	}
	else if (op == 4) {
		//MultiplyWithAlpha
		vec3 color1 = colorSample0.rgb;
		float alpha1 = colorSample0.a;
		fragColor.rgb =  color1 * alpha1;
		fragColor.a = 1.0;
	}
	else if (op == 5) {
		//BlendWithAlpha
		vec3 color1 = colorSample0.rgb;
		vec3 color2 = colorSample1.rgb;
		float alpha1 = colorSample0.a;
		float alpha2 = colorSample1.a;
		fragColor.rgb =  color1/**alpha1*/ * (1.0f-blendFactor) + color2/**alpha2*/ * blendFactor;
		fragColor.a = alpha1 * (1.0-blendFactor) + alpha2 * blendFactor;
	}	
	else if (op == 6) {
		//Show alpha
		vec3 color1 = colorSample0.rgb;
		float alpha1 = colorSample0.a;
		fragColor.rgb =  vec3(1.0,1.0,1.0) * alpha1;
		fragColor.a = 1.0f;
	}
	else if (op == 7) {
		//Invert alpha
		vec3 color1 = colorSample0.rgb;
		float alpha1 = colorSample0.a;
		fragColor.rgb =  color1;
		fragColor.a = 1.0-alpha1;
	}
	else if (op == 8) {
		//PassThrough
		vec3 color1 = colorSample0.rgb;
		float alpha1 = colorSample0.a;
		fragColor.rgb =  color1;
		fragColor.a = alpha1;
	}	
    else if (op == 9) {
		//B over A
        vec4 src = colorSample1;
        vec4 dst = colorSample0;
        float alpha = src.a*blendFactor;
        fragColor = alpha*src + (1.0-alpha)*dst;
	}
	
	//fragColor = color;
}