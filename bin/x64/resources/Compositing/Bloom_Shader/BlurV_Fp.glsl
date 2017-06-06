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

//////////////////////////////////////////////
// Horizontal/Vertical Gaussian-Blur passes //
//////////////////////////////////////////////


//We use the Normal-gauss distribution formula
//f(x) being the formula, we used f(0.5)-f(-0.5); f(1.5)-f(0.5)...

vec2 samples[11] = vec2[11] (
	vec2(-0.05, 0.01222447), 
	vec2(-0.04, 0.02783468),	
	vec2(-0.03, 0.06559061),	
	vec2(-0.02, 0.12097757),	
	vec2(-0.01, 0.17466632),	
	vec2(0.00, 0.19741265),	
	vec2(0.01, 0.17466632),	
	vec2(0.02, 0.12097757),	
	vec2(0.03, 0.06559061),	
	vec2(0.04, 0.02783468),	
	vec2(0.05, 0.01222447)
);

in vec4 vertexPosition;
in vec2 oTexCoord;
out vec4 fragColor;
uniform sampler2D Blur;
uniform float BlurWidthV;

void main()
{
	vec4 result = vec4(0);
	
	for (int i = 0; i < 11; i++)
		result += texture(Blur, vec2(oTexCoord + vec2(0.0, samples[i].x*BlurWidthV))) * samples[i].y;
	
	fragColor = result;
}


