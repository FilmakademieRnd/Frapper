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

in  vec2 oTexCoord;			
out vec4 fragColor;

uniform sampler2D inputMap;
uniform float brightness;
uniform float contrast;

uniform float hue;
uniform float saturation;

void main (void)
{
	vec4 pixelColor = vec4(texture2D(inputMap, oTexCoord));
	pixelColor.rgb /= pixelColor.a;
	
	//Contrast
	pixelColor.rgb = ( (pixelColor.rgb - 0.5) * max(contrast, 0) ) + 0.5;
	
	//Brightness
	pixelColor.rgb *= brightness;
	
	//Final pixel color
	pixelColor.rgb *= pixelColor.a;

	fragColor = pixelColor;

}

// void main(void)
// {
	// vec4 pixelColor = vec4(texture2D(inputMap, oTexCoord));
	
	// vec3 hsv;
	// vec3 intensity;
	// vec3 root3 = vec3(0.57735, 0.57735, 0.57735);
	// float half_angle = 0.5 * radians(hue); 
// }
