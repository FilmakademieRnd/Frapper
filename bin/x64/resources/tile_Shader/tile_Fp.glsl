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

/////////////////
// Tile shader //
/////////////////

in  vec2 oTexCoord;
out vec4 oColor;
uniform float ImageIndex;
uniform float TilesX;
uniform float TilesY;
uniform sampler2D tiles;

void main()
{
	float off_x = mod( ImageIndex, TilesX );
	float off_y = floor( ImageIndex / TilesX );

	vec2 tileCoord;
	tileCoord.x = (off_x * 1.0/TilesX) + (oTexCoord.x / TilesX);
	tileCoord.y = (off_y * 1.0/TilesY) + (oTexCoord.y / TilesY);

	oColor = texture2D( tiles, tileCoord);
	//oColor = float4( tileCoord.x, tileCoord.y, 0.0, 1.0 );
}