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

//#define DEBUG_COLOR

in float strandWidth[];
in vec3 ds_tangent[];
in float ds_strandPosition[];

#ifdef DEBUG_COLOR
in vec4 color[];

out vec4 debugColor;
#endif

out vec3 gs_tangent;
out vec3 gs_viewDir;
out vec3 gs_wvPos;
out float gs_strandPosition;

uniform mat4 g_mWorldViewProjection;
uniform mat4 g_mWorldView;
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
	
	vec3 eyeVec = g_CameraPosition.xyz - pos1.xyz;
	vec3 sideVec = normalize(cross(eyeVec, tangent));
	
	vec3 width1 = sideVec * strandWidth[0];
	vec3 width2 = sideVec * strandWidth[1];
	
	// Offset positions to for drawing triangles in world space
	vec4 pos11 = vec4( pos1.xyz + width1, 1 );
	vec4 pos12 = vec4( pos1.xyz - width1, 1 );
	
	vec4 pos21 = vec4( pos2.xyz + width2, 1 );
	vec4 pos22 = vec4( pos2.xyz - width2, 1 );
	//-----------------------------------------------------
	
	gl_Position = g_mWorldViewProjection * pos11;
	gs_wvPos = (g_mWorldView * pos11).xyz;
#ifdef DEBUG_COLOR
	debugColor = color[0];
	debugColor  = vec4(debugColor.r * (0.0), debugColor.g * (0.0) , debugColor.b * (0.0) ,1); // Colors are adjusted with * 0.0 to get a slight gradient per segment for debugging
#endif
	gs_tangent = ds_tangent[0];
	gs_viewDir = normalize(g_CameraPosition.xyz - pos11.xyz);
	gs_strandPosition = ds_strandPosition[0];
	EmitVertex();

	gl_Position = g_mWorldViewProjection * pos12;
	gs_wvPos = (g_mWorldView * pos12).xyz;
#ifdef DEBUG_COLOR
	debugColor = color[0];
	debugColor  = vec4(debugColor.r * (0.33), debugColor.g * (0.33) , debugColor.b * (0.33) ,1); // Colors are adjusted with * 0.33 to get a slight gradient per segment for debugging
#endif
	//gs_tangent = ds_tangent[0];
	gs_viewDir = normalize(g_CameraPosition.xyz - pos12.xyz);
	gs_strandPosition = ds_strandPosition[0];
	EmitVertex();

	gl_Position = g_mWorldViewProjection * pos21;
	gs_wvPos = (g_mWorldView * pos21).xyz;
#ifdef DEBUG_COLOR
	debugColor = color[0];
	debugColor  = vec4(debugColor.r * (0.66), debugColor.g * (0.66) , debugColor.b * (0.6) ,1); // Colors are adjusted with * 0.66 to get a slight gradient per segment for debugging
#endif
	gs_tangent = ds_tangent[1];
	gs_viewDir = normalize(g_CameraPosition.xyz - pos21.xyz);
	gs_strandPosition = ds_strandPosition[1];
	EmitVertex();
	
	gl_Position = g_mWorldViewProjection * pos22;
	gs_wvPos = (g_mWorldView * pos22).xyz;
#ifdef DEBUG_COLOR
	debugColor = color[0];
	debugColor  = vec4(debugColor.r * (1.0), debugColor.g * (1.0) , debugColor.b * (1.0) ,1); // Colors are adjusted with * 1.0 to get a slight gradient per segment for debugging
#endif
	//gs_tangent = * ds_tangent[1];
	gs_viewDir = normalize(g_CameraPosition.xyz - pos22.xyz);
	gs_strandPosition = ds_strandPosition[1];
	EmitVertex();

	EndPrimitive();
}