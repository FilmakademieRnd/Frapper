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
**                                  colorcorrection_ps								 **
**                                                                                   **
***************************************************************************************/
#version 400 core

out vec4 oColor;
in vec2 oTexCoord;
uniform sampler2D inputMap;
uniform sampler2D lutMap;

void main()
{
	oColor = texture(inputMap, oTexCoord);

	oColor.r = texture(lutMap, vec2(oColor.r, .0f)).r;
	oColor.g = texture(lutMap, vec2(oColor.g, .0f)).g;
	oColor.b = texture(lutMap, vec2(oColor.b, .0f)).b;
	oColor.a = texture(lutMap, vec2(oColor.a, .0f)).a;

	// 4 debug
	//oColor = float4(tex2D(lutMap, texCoord).rgb, 1.0);
}