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
// LenseDistortion //
/////////////////////
in  vec2 oTexCoord;
out vec4 oColor;
uniform float distortionFactor;
uniform float focalLengthX;
uniform float focalLengthY;
uniform float principalPointX;
uniform float principalPointY;
uniform float scale;
uniform sampler2D sceneRT;

void main()
{
	vec2 principalPoint = vec2(principalPointX, principalPointY); //the centere of the distortion
	vec2 focalLength = vec2(focalLengthX, focalLengthY);

	vec2 texCoord = (oTexCoord - principalPoint)/(scale*focalLength);
	float rDistored = sqrt(texCoord.x*texCoord.x + texCoord.y*texCoord.y); //the distortedRadius
	float rUndesitored = tan(rDistored*distortionFactor)/(2.0*tan(distortionFactor/2.0)); //the UndistortedRadius
	vec2 distored = (rDistored / rUndesitored) * texCoord  * focalLength + principalPoint;
		
	oColor = texture2D(sceneRT, distored);
}