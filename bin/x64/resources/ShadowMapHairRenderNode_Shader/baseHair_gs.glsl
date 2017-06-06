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


in float strandWidth[];
in vec3 ds_tangent[];
in float ds_strandPosition[];

out float gs_Col;

uniform mat4 g_mInverseWorldMatrix;
uniform mat4 g_mWorldViewProjection;
uniform vec4 g_CameraPosition;

layout(lines) in;
layout(triangle_strip, max_vertices = 6) out;

void main() {

	vec4 pos1 = gl_in[0].gl_Position;
	vec4 pos2 = gl_in[1].gl_Position;

	// Expand line to quad
	//-----------------------------------------------------
	// Tangent of the line segment
	vec3 tangent = pos2.xyz - pos1.xyz;
	tangent = normalize(tangent);
	
	vec3 eyeVec = (g_mInverseWorldMatrix * g_CameraPosition).xyz - pos1.xyz;
	vec3 sideVec = normalize(cross(eyeVec, tangent));
	
	vec3 width1 = sideVec * strandWidth[0];
	vec3 width2 = sideVec * strandWidth[1];
	
	// Offset positions to for drawing triangles in world space
	vec4 pos11 = vec4( pos1.xyz + width1, 1 );
	vec4 pos12 = vec4( pos1.xyz - width1, 1 );
	
	vec4 pos21 = vec4( pos2.xyz + width2, 1 );
	vec4 pos22 = vec4( pos2.xyz - width2, 1 );
	//-----------------------------------------------------
	
	
	// Move the vertex-positions into the final space
	pos11 = g_mWorldViewProjection * pos11;
	pos12 = g_mWorldViewProjection * pos12;

	pos21 = g_mWorldViewProjection * pos21;
	pos22 = g_mWorldViewProjection * pos22;
	
	gl_Position = pos11;
	gs_Col = 0;
	EmitVertex();

	gl_Position = pos12;
	gs_Col = 0;
	EmitVertex();

	gl_Position = pos21;
	gs_Col = 0;
	EmitVertex();
	
	gl_Position = pos22;
	gs_Col = 0;
	EmitVertex();

	EndPrimitive();
}