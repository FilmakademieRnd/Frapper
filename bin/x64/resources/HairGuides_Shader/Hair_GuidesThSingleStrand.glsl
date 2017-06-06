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

//#extension GL_ARB_tessellation_shader: enable

// This allows us to compile the shader with a #define to choose
// the different partition modes for the hull shader.

// See the hull shader: [partitioning(BEZIER_HS_PARTITION)]
// This sample demonstrates "integer", "fractional_even", and "fractional_odd"

#define TCS_OUTPUT_SIZE 4

// GLSL Control shader
layout (vertices = TCS_OUTPUT_SIZE) out;

in gl_PerVertex{
	vec4 gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[];
} gl_in[gl_MaxPatchVertices];

out gl_PerVertex{
	vec4 gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[];
} gl_out[];


vec2 textureCoordinates2D(uint readPos, float textureSize)
{
	vec2 textureCoordinates;
	
	// Calculate position in texture buffer to read 3D vertex position from there
	textureCoordinates.x = ( readPos % uint(textureSize) ) / textureSize;			// column
	textureCoordinates.y = floor( readPos / uint(textureSize) ) / textureSize;		// row
	
	return textureCoordinates;
}

out vec3 hs_tangent[];

// coordinate frames
out vec3 hs_yAxisCF[];
out vec3 hs_zAxisCF[];

out float hs_CurrentNumOfGuideHairSegments[];


uniform float g_tessellationFactor;
uniform float g_detailFactor;
uniform float g_textureSize;
uniform float g_coordinateFramesTextureSize;

uniform sampler2D Tex0;
uniform sampler2D TexTangents;
uniform sampler2D TexCoordinateFrames;


void main()
{
	vec4 position[TCS_OUTPUT_SIZE];
	vec3 yAxisCF[TCS_OUTPUT_SIZE];
	vec3 zAxisCF[TCS_OUTPUT_SIZE];

	vec2 textureCoordinate1 = textureCoordinates2D(gl_PrimitiveID, g_textureSize);
	vec4 position1 = texture(Tex0, textureCoordinate1 );
	
	if( position1.w == 0.0 )
	{
		// end of strand don't render
		gl_TessLevelOuter[0] = 0.0;
		gl_TessLevelOuter[1] = 0.0;
	}
	else
	{
		// set tessellation factors
		gl_TessLevelOuter[0] = g_tessellationFactor;
		gl_TessLevelOuter[1] = g_detailFactor;

		vec4 tangent1 = texture(TexTangents, textureCoordinate1);
		
		float currentNumOfGuideHairSegments[4];
		currentNumOfGuideHairSegments[0] = tangent1.w;
		currentNumOfGuideHairSegments[1] = tangent1.w;
		currentNumOfGuideHairSegments[2] = tangent1.w;
		currentNumOfGuideHairSegments[3] = tangent1.w;

		if(position1.w == tangent1.w)
		{
			// first segment
			
			// positions
			position[0] = position[1] = position1;
			position[2] = texture(Tex0, textureCoordinates2D(gl_PrimitiveID + 1, g_textureSize));
			position[3] = texture(Tex0, textureCoordinates2D(gl_PrimitiveID + 2, g_textureSize));
			
			// coordinate frames
			yAxisCF[0] = yAxisCF[1] = texture(TexCoordinateFrames, textureCoordinates2D(  gl_PrimitiveID * 3 + 1, g_coordinateFramesTextureSize)).xyz;
			zAxisCF[0] = zAxisCF[1] = texture(TexCoordinateFrames, textureCoordinates2D(  gl_PrimitiveID * 3 + 2, g_coordinateFramesTextureSize)).xyz;
			
			yAxisCF[2] = texture(TexCoordinateFrames, textureCoordinates2D(  (gl_PrimitiveID + 1) * 3 + 1, g_coordinateFramesTextureSize)).xyz;
			zAxisCF[2] = texture(TexCoordinateFrames, textureCoordinates2D(  (gl_PrimitiveID + 1) * 3 + 2, g_coordinateFramesTextureSize)).xyz;
			
			yAxisCF[3] = texture(TexCoordinateFrames, textureCoordinates2D(  (gl_PrimitiveID + 2) * 3 + 1, g_coordinateFramesTextureSize)).xyz;
			zAxisCF[3] = texture(TexCoordinateFrames, textureCoordinates2D(  (gl_PrimitiveID + 2) * 3 + 2, g_coordinateFramesTextureSize)).xyz;
		}
		else if(position1.w == 1)
		{
			// last segment
			
			// positions
			position[0] =  texture(Tex0, textureCoordinates2D(gl_PrimitiveID - 1, g_textureSize));
			position[1] = position1;
			position[2] = position[3] =  texture(Tex0, textureCoordinates2D(gl_PrimitiveID + 1, g_textureSize));
			
			// coordinate frames
			yAxisCF[0] = texture(TexCoordinateFrames, textureCoordinates2D(  (gl_PrimitiveID - 1) * 3 + 1, g_coordinateFramesTextureSize)).xyz;
			zAxisCF[0] = texture(TexCoordinateFrames, textureCoordinates2D(  (gl_PrimitiveID - 1) * 3 + 2, g_coordinateFramesTextureSize)).xyz;
			
			yAxisCF[1] = texture(TexCoordinateFrames, textureCoordinates2D(  (gl_PrimitiveID + 0) * 3 + 1, g_coordinateFramesTextureSize)).xyz;
			zAxisCF[1] = texture(TexCoordinateFrames, textureCoordinates2D(  (gl_PrimitiveID + 0) * 3 + 2, g_coordinateFramesTextureSize)).xyz;
			
			yAxisCF[2] = yAxisCF[3] = texture(TexCoordinateFrames, textureCoordinates2D(  (gl_PrimitiveID + 1) * 3 + 1, g_coordinateFramesTextureSize)).xyz;
			zAxisCF[2] = zAxisCF[3] = texture(TexCoordinateFrames, textureCoordinates2D(  (gl_PrimitiveID + 1) * 3 + 2, g_coordinateFramesTextureSize)).xyz;
		}
		else
		{
			// positions
			position[0] = texture(Tex0, textureCoordinates2D(gl_PrimitiveID - 1, g_textureSize));
			position[1] = position1;
			position[2] = texture(Tex0, textureCoordinates2D(gl_PrimitiveID + 1, g_textureSize));
			position[3] = texture(Tex0, textureCoordinates2D(gl_PrimitiveID + 2, g_textureSize));
			
			// coordinate frames
			yAxisCF[0] = texture(TexCoordinateFrames, textureCoordinates2D( (gl_PrimitiveID - 1) * 3 + 1, g_coordinateFramesTextureSize)).xyz;
			zAxisCF[0] = texture(TexCoordinateFrames, textureCoordinates2D( (gl_PrimitiveID - 1) * 3 + 2, g_coordinateFramesTextureSize)).xyz;
			
			yAxisCF[1] = texture(TexCoordinateFrames, textureCoordinates2D( (gl_PrimitiveID + 0) * 3 + 1, g_coordinateFramesTextureSize)).xyz;
			zAxisCF[1] = texture(TexCoordinateFrames, textureCoordinates2D( (gl_PrimitiveID + 0) * 3 + 2, g_coordinateFramesTextureSize)).xyz;
			
			yAxisCF[2] = texture(TexCoordinateFrames, textureCoordinates2D( (gl_PrimitiveID + 1) * 3 + 1, g_coordinateFramesTextureSize)).xyz;
			zAxisCF[2] = texture(TexCoordinateFrames, textureCoordinates2D( (gl_PrimitiveID + 1) * 3 + 2, g_coordinateFramesTextureSize)).xyz;
			
			yAxisCF[3] = texture(TexCoordinateFrames, textureCoordinates2D( (gl_PrimitiveID + 2) * 3 + 1, g_coordinateFramesTextureSize)).xyz;
			zAxisCF[3] = texture(TexCoordinateFrames, textureCoordinates2D( (gl_PrimitiveID + 2) * 3 + 2, g_coordinateFramesTextureSize)).xyz;
		}
		
		// calculate tangents
		vec3 tangent[4];
		tangent[0] = position[1].xyz - position[0].xyz;
		tangent[1] = position[2].xyz - position[1].xyz;
		tangent[2] = position[3].xyz - position[2].xyz;
		tangent[3] = vec3(0.0, 0.0, 0.0);

		// put through domain/evalution shader
		gl_out[gl_InvocationID].gl_Position = position[gl_InvocationID];
		hs_tangent[gl_InvocationID] = tangent[gl_InvocationID];
		hs_yAxisCF[gl_InvocationID] = yAxisCF[gl_InvocationID];
		hs_zAxisCF[gl_InvocationID] = zAxisCF[gl_InvocationID];
		hs_CurrentNumOfGuideHairSegments[gl_InvocationID] = currentNumOfGuideHairSegments[gl_InvocationID];
	}
}

