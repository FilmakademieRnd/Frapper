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

in  vec2 oTexCoord;
out vec4 oColor;
uniform float focalPlane;
uniform float radius;
uniform sampler2D inputMap0;
uniform sampler2D inputMap1;
uniform sampler2D inputMap2;

void main()
{
	vec4 fragColor = texture2D(inputMap0, oTexCoord);
	vec4 fragBlur = texture2D(inputMap1, oTexCoord);
	float fragDepth = texture2D(inputMap2, oTexCoord).r;
	float offFocus = clamp(abs(fragDepth - focalPlane) / radius, 0.0, 1.0);
	oColor = mix(fragColor, fragBlur, offFocus);
}