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

uniform sampler2D inputMap;
uniform vec4 pixelSize;


varying vec4 vertexScreenSpace;
				
											 
void main (void)
{
	vec2 texCoord = vertexScreenSpace.xy / vertexScreenSpace.w * 0.5 + vec2(0.5);
	
	vec4 value = vec4(0.0, 0.0, 0.0, 0.0);
	float normalization = 0.0;
	
	//sample (-1,-1)
	vec4 domainValue = texture2D(inputMap, texCoord + vec2(-1.0 * pixelSize.x, -1.0 * pixelSize.y));
	value += domainValue * 0.36788;
	
	//sample (0,-1)
	domainValue = texture2D(inputMap, texCoord + vec2( 0.0 * pixelSize.x, -1.0 * pixelSize.y));
	value += domainValue * 0.60653;
	
	//sample (1,-1)
	domainValue = texture2D(inputMap, texCoord + vec2(+1.0 * pixelSize.x, -1.0 * pixelSize.y));
	value += domainValue * 0.36788;
	
	//sample (-1,0)
	domainValue = texture2D(inputMap, texCoord + vec2(-1.0 * pixelSize.x, 0.0 * pixelSize.y));
	value += domainValue * 0.60653;
	
	//sample (0,0)
	domainValue = texture2D(inputMap, texCoord + vec2( 0.0 * pixelSize.x, 0.0 * pixelSize.y));
	value += domainValue * 1.0;
	
	//sample (+1,0)
	domainValue = texture2D(inputMap, texCoord + vec2(+1.0 * pixelSize.x, 0.0 * pixelSize.y));
	value += domainValue * 0.60653;
				
	//sample (-1,+1)		
	domainValue = texture2D(inputMap, texCoord + vec2(-1.0 * pixelSize.x, +1.0 * pixelSize.y));
	value += domainValue * 0.36788;			
	
	//sample (0,+1)		
	domainValue = texture2D(inputMap, texCoord + vec2( 0.0 * pixelSize.x, +1.0 * pixelSize.y));
	value += domainValue * 0.60653;			
	
	//sample (+1,+1)		
	domainValue = texture2D(inputMap, texCoord + vec2(+1.0 * pixelSize.x, +1.0 * pixelSize.y));
	value += domainValue * 0.36788;	


	normalization = 4.89764;
	gl_FragColor = value / normalization;
}

