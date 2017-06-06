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
uniform float domainRadius;


void main (void)
{
	vec2 texCoord = vertexScreenSpace.xy / vertexScreenSpace.w * 0.5 + vec2(0.5);

	float omega = domainRadius * 0.5;
	
	vec4 value = vec4(0.0, 0.0, 0.0, 0.0);
	
	float normalization = 0.0;
	
	if (domainRadius >= 1.0)
	{
		for (int x = -int(domainRadius); x <= int(domainRadius); x++)
		{
			vec2 samplePos = vec2(float(x) * pixelSize.x, 0.0);

			vec4 domainValue = texture2D(inputMap, texCoord + samplePos);
			
			float powValue = abs(float(x)) / omega;
			float domainWeight = exp( -0.5 * powValue * powValue );
			
			value += domainValue * domainWeight;
			normalization += domainWeight;			
		}
		
		fragColor = value / normalization;
	}
	else
	{
		fragColor = texture2D(inputMap, texCoord); 
	}
}

