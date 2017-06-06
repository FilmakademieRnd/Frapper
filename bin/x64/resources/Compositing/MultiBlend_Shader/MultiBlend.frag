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

//////////////////////////////////////////////
// MultiBlend shader //
//////////////////////////////////////////////

/////////////////
// Blend-Pass //
/////////////////
#version 400 core

in vec2 oTexCoord;			//: oTexCoord0,
out vec4 fragColor;		//: COLOR0,

uniform sampler2D RT0;		//: TEXUNIT0,
uniform sampler2D RT1;		//: TEXUNIT1,
uniform sampler2D RT2;		//: TEXUNIT2,
uniform sampler2D RT3;		//: TEXUNIT3,
uniform sampler2D RT4;		//: TEXUNIT4,
uniform sampler2D RT5;		//: TEXUNIT5,
uniform sampler2D RT6;		//: TEXUNIT6,
uniform sampler2D RT7;		//: TEXUNIT7,
uniform sampler2D RT8;		//: TEXUNIT8,			   
uniform sampler2D RT9;		//: TEXUNIT9,			   
uniform sampler2D RT10;		//: TEXUNIT10,			   
uniform sampler2D RT11;		//: TEXUNIT11,			   
uniform sampler2D RT12;		//: TEXUNIT12,			   
uniform sampler2D RT13;		//: TEXUNIT13,			   
uniform sampler2D RT14;		//: TEXUNIT14,			   
uniform sampler2D RT15;		//: TEXUNIT15,		   
uniform float ImageIndex;
			   
void main()
{
	vec4 c = texture2D(RT0, oTexCoord);
	
	if (ImageIndex == 1.0)
		c = texture2D(RT1, oTexCoord);
	else if (ImageIndex == 2.0)
		c = texture2D(RT2, oTexCoord);
	else if (ImageIndex == 3.0)
		c = texture2D(RT3, oTexCoord);
	else if (ImageIndex == 4.0)
		c = texture2D(RT4, oTexCoord);
	else if (ImageIndex == 5.0)
		c = texture2D(RT5, oTexCoord);
	else if (ImageIndex == 6.0)
		c = texture2D(RT6, oTexCoord);
	else if (ImageIndex == 7.0)
		c = texture2D(RT7, oTexCoord);
	else if (ImageIndex == 8.0)
		c = texture2D(RT8, oTexCoord);
	else if (ImageIndex == 9.0)
		c = texture2D(RT9, oTexCoord);
	else if (ImageIndex == 10.0)
		c = texture2D(RT10, oTexCoord);
	else if (ImageIndex == 11.0)
		c = texture2D(RT11, oTexCoord);
	else if (ImageIndex == 12.0)
		c = texture2D(RT12, oTexCoord);
	else if (ImageIndex == 13.0)
		c = texture2D(RT13, oTexCoord);
	else if (ImageIndex == 14.0)
		c = texture2D(RT14, oTexCoord);
	else if (ImageIndex == 15.0)
		c = texture2D(RT15, oTexCoord);	
		
	fragColor = c;
	//fragColor = vec4(oTexCoord,0,1);
}