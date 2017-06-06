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

#define PI 3.1415926535897932384626433832795

//#define SCREEN_SPACE_ADAPTIVE_TESSELLATION

#define TCS_OUTPUT_SIZE 12

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

// coordinate frames
out vec3 hs_yAxisCF[];
out vec3 hs_zAxisCF[];

// per patch outputs
patch out int  hs_calculateClumpPosition;
patch out float hs_clumpTransitionStart;
patch out float hs_strandPosition;
patch out float hs_currentNumOfGuideHairSegments;
patch out float hs_currentNumOfGuideHairSegmentsArray[3];

uniform mat4 g_mViewProjection;
uniform vec4 g_viewDirection;
uniform float g_screenResolutionX; 
uniform float g_screenResolutionY;
uniform float g_tessellationFactor;
uniform float g_detailFactor;
uniform float g_textureSize;
uniform float g_textureSizeIndices;
uniform float g_maxDistance;
uniform float g_maxAngle;
uniform float g_coordinateFramesTextureSize;
uniform float g_maxDistanceAngleTextureSize;
uniform float g_scalpNormalsTextureSize;
uniform float g_backFaceCullingScale;
uniform float g_screenSpaceTessellationScale;
uniform float g_screenSpaceAdpativeTessellation;

uniform sampler2D Tex0;
uniform sampler2D TexTangents;
uniform sampler2D TexCoordinateFrames;
uniform sampler2D TexIndices;
uniform sampler2D TexIndicesEnd;
uniform sampler2D TexMaxDistanceAngle;
uniform sampler2D TexSculpNormalIndices;
uniform sampler2D TexSculpNormals;
uniform sampler2D TexStrandBend;


float angleBetween(vec3 vector1, vec3 vector2)
{
	return degrees( acos( dot(vector1, vector2) / ( length(vector1) * length(vector2) ) ) );
}

vec4 readTextureValue(sampler2D textureName, float textureSize, uint readPos)
{
	float colNr = ( readPos % uint(textureSize) ) / textureSize;
	float rowNr = floor( readPos / uint(textureSize) ) / textureSize;
	
	return texture(textureName, vec2(colNr, rowNr));
}

vec2 textureCoordinates2D(float readPos, float textureSize)
{
	uint readPosUI = uint(readPos);
	vec2 textureCoordinates;
	
	// Calculate position in texture buffer to read 3D vertex position from there
	textureCoordinates.x = ( readPosUI % uint(textureSize) ) / textureSize;			// column
	textureCoordinates.y = floor( readPosUI / uint(textureSize) ) / textureSize;		// row
	
	return textureCoordinates;
}

//--------------------------------------------------------------------------------------
// Returns the dot product between the viewing vector and the patch edge
//--------------------------------------------------------------------------------------
float GetEdgeDotProduct ( 
                        vec3 f3EdgeNormal0,   // Normalized normal of the first control point of the given patch edge 
                        vec3 f3EdgeNormal1   // Normalized normal of the second control point of the given patch edge 
                        )
{
    vec3 f3EdgeNormal = normalize( ( f3EdgeNormal0 + f3EdgeNormal1 ) * 0.5f );
    
    float fEdgeDotProduct = dot( f3EdgeNormal, g_viewDirection.xyz );

    return fEdgeDotProduct;
}

//--------------------------------------------------------------------------------------
// Returns back face culling test result (true / false)
//--------------------------------------------------------------------------------------
bool BackFaceCull    ( 
                    float fEdgeDotProduct0, 		// Dot product of edge 0 normal with view vector
                    float fEdgeDotProduct1, 		// Dot product of edge 1 normal with view vector
                    float fEdgeDotProduct2, 		// Dot product of edge 2 normal with view vector
                    float fBackFaceCullingScale  	// Scale of backface culling
                    )
{
    vec3 f3BackFaceCull;
	
	float fBackFaceEpsilon = 1.0 - fBackFaceCullingScale; 
    
    f3BackFaceCull.x = ( fEdgeDotProduct0 > fBackFaceEpsilon ) ? ( 0.0f ) : ( 1.0f );
    f3BackFaceCull.y = ( fEdgeDotProduct1 > fBackFaceEpsilon ) ? ( 0.0f ) : ( 1.0f );
    f3BackFaceCull.z = ( fEdgeDotProduct2 > fBackFaceEpsilon ) ? ( 0.0f ) : ( 1.0f );
    
	if( f3BackFaceCull.x != 0.0f &&
		f3BackFaceCull.y != 0.0f &&
		f3BackFaceCull.z != 0.0f)
	{
		return false;
	}
	else
	{
		return true;
	}
}

//--------------------------------------------------------------------------------------
// Screen Space Adaptive Tessellation
//--------------------------------------------------------------------------------------
// Returns the screen space position from the given world space patch control point
vec2 GetScreenSpacePosition( vec3 f3Position )            // Screen height
{
	vec4 f4ProjectedPosition = g_mViewProjection * vec4( f3Position, 1.0f );

	vec2 f2ScreenPosition = f4ProjectedPosition.xy / f4ProjectedPosition.ww;

	f2ScreenPosition = ( f2ScreenPosition + 1.0f ) * 0.5f * vec2( g_screenResolutionX, -g_screenResolutionY );

	return f2ScreenPosition;
}


void main()
{
	vec4 position[TCS_OUTPUT_SIZE];
	vec3 yAxisCF[TCS_OUTPUT_SIZE];
	vec3 zAxisCF[TCS_OUTPUT_SIZE];

	// normal index list
	uint readPos1 = uint(gl_PrimitiveID);
	vec4 normalIDs = readTextureValue(TexSculpNormalIndices, g_textureSizeIndices, readPos1);
	// read scalp mesh normals
	vec3 normal1 = readTextureValue(TexSculpNormals, g_scalpNormalsTextureSize, uint(normalIDs.x) ).xyz;
	vec3 normal2 = readTextureValue(TexSculpNormals, g_scalpNormalsTextureSize, uint(normalIDs.y) ).xyz;
	vec3 normal3 = readTextureValue(TexSculpNormals, g_scalpNormalsTextureSize, uint(normalIDs.z) ).xyz;

	// Perform back face culling test
	// Aquire patch edge dot product between patch edge normal and view vector
	float fEdgeDot[3];
	fEdgeDot[0] = GetEdgeDotProduct( normal3, normal1);
	fEdgeDot[1] = GetEdgeDotProduct( normal1, normal2);
	fEdgeDot[2] = GetEdgeDotProduct( normal2, normal3);

	// If all 3 fail the test then back face cull
	if(BackFaceCull( fEdgeDot[0], fEdgeDot[1], fEdgeDot[2], g_backFaceCullingScale ))
	{
		// Set all tessellation factors to 0 if all vertices of the patch are culled
		gl_TessLevelOuter[0] = 0.0;
		gl_TessLevelOuter[1] = 0.0;
	}
	else
	{
		// set tessellation factors
		gl_TessLevelOuter[0] = g_tessellationFactor;
		gl_TessLevelOuter[1] = g_detailFactor;

		// Read the begin and end vertex positions of the line segment from texture
		vec4 strandIDs1 = readTextureValue(TexIndices, g_textureSizeIndices, readPos1);
		vec4 strandIDs2 = readTextureValue(TexIndicesEnd, g_textureSizeIndices, readPos1);
		
		// positions
		// position 1
		vec4 position11 = readTextureValue(Tex0, g_textureSize, uint( strandIDs1.x ) );
		vec4 position12 = readTextureValue(Tex0, g_textureSize, uint( strandIDs1.y ) );
		vec4 position13 =  readTextureValue(Tex0, g_textureSize, uint( strandIDs1.z ) );
		
		// read for hair segment size
		// TODO: remove tangents
		vec4 tangent11 = readTextureValue(TexTangents, g_textureSize, uint( strandIDs1.x ) );
		vec4 tangent12 = readTextureValue(TexTangents, g_textureSize, uint( strandIDs1.y ) );
		vec4 tangent13 = readTextureValue(TexTangents, g_textureSize, uint( strandIDs1.z ) );
		
		// position of hair strand segemend and size of hair strand
		hs_strandPosition = position11.w;
		hs_currentNumOfGuideHairSegments = tangent11.w;
		if(tangent12.w < hs_currentNumOfGuideHairSegments)
		{
			hs_currentNumOfGuideHairSegments = tangent12.w;
			hs_strandPosition = position12.w;
		}
		if(tangent13.w < hs_currentNumOfGuideHairSegments)
		{
			hs_currentNumOfGuideHairSegments = tangent13.w;
			hs_strandPosition = position13.w;
		}
		
		hs_currentNumOfGuideHairSegmentsArray[0] = tangent11.w;
		hs_currentNumOfGuideHairSegmentsArray[1] = tangent12.w;
		hs_currentNumOfGuideHairSegmentsArray[2] = tangent13.w;
		
		// Check if we will need to calculate clump position for single strand rendering
		// read maximum distance/angle between vertices
		uint readPosGuide = uint(gl_PrimitiveID / tangent11.w);
		vec4 maxDistanceAngle = readTextureValue(TexMaxDistanceAngle, g_maxDistanceAngleTextureSize, readPosGuide);
		
		bool calculateClumpPosition = false;
		hs_calculateClumpPosition = 0;
		if( maxDistanceAngle.x > g_maxDistance )
		{
			calculateClumpPosition = true;
			hs_calculateClumpPosition = 1;
			hs_clumpTransitionStart = maxDistanceAngle.y;
		}
		else
		{
			if( maxDistanceAngle.z > g_maxAngle )
			{
				calculateClumpPosition = true;
				hs_calculateClumpPosition = 1;
				hs_clumpTransitionStart = maxDistanceAngle.w;
			}
		}
		
		for( int i=0; i<TCS_OUTPUT_SIZE; i++)
		{
			//position[i] = vec4(0,0,0,1);
			yAxisCF[i] = vec3(0,0,0);
			zAxisCF[i] = vec3(0,0,0);
		}
		
		if( position11.w == tangent11.w )
		{
			// first segment
			uint readPos3 = uint(gl_PrimitiveID + 1);		// 2-3
			vec4 strandIDs3 = readTextureValue(TexIndicesEnd, g_textureSizeIndices, readPos3);
			
			// position 1
			position[0] = position[3] = position11;
			position[1] = position[4] = position12;
			position[2] = position[5] = position13;
			// position 2
			position[6] = readTextureValue(Tex0, g_textureSize, uint( strandIDs2.x ) );
			position[7] = readTextureValue(Tex0, g_textureSize, uint( strandIDs2.y ) );
			position[8] = readTextureValue(Tex0, g_textureSize, uint( strandIDs2.z ) );
			// position 3
			position[9]  = readTextureValue(Tex0, g_textureSize, uint( strandIDs3.x ) );
			position[10] = readTextureValue(Tex0, g_textureSize, uint( strandIDs3.y ) );
			position[11] = readTextureValue(Tex0, g_textureSize, uint( strandIDs3.z ) );
			
			if( calculateClumpPosition )
			{
				// coordinate frames
				// CF1
				yAxisCF[0] = yAxisCF[3] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs1.x * 3 + 1, g_coordinateFramesTextureSize)).xyz;
				zAxisCF[0] = zAxisCF[3] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs1.x * 3 + 2, g_coordinateFramesTextureSize)).xyz;
				yAxisCF[1] = yAxisCF[4] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs1.y * 3 + 1, g_coordinateFramesTextureSize)).xyz;
				zAxisCF[1] = zAxisCF[4] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs1.y * 3 + 2, g_coordinateFramesTextureSize)).xyz;
				yAxisCF[2] = yAxisCF[5] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs1.z * 3 + 1, g_coordinateFramesTextureSize)).xyz;
				zAxisCF[2] = zAxisCF[5] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs1.z * 3 + 2, g_coordinateFramesTextureSize)).xyz;
				
				// CF2
				yAxisCF[6] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs2.x * 3 + 1, g_coordinateFramesTextureSize)).xyz;
				zAxisCF[6] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs2.x * 3 + 2, g_coordinateFramesTextureSize)).xyz;
				yAxisCF[7] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs2.y * 3 + 1, g_coordinateFramesTextureSize)).xyz;
				zAxisCF[7] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs2.y * 3 + 2, g_coordinateFramesTextureSize)).xyz;
				yAxisCF[8] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs2.z * 3 + 1, g_coordinateFramesTextureSize)).xyz;
				zAxisCF[8] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs2.z * 3 + 2, g_coordinateFramesTextureSize)).xyz;
				
				// CF3
				yAxisCF[9] = texture(TexCoordinateFrames, textureCoordinates2D(  strandIDs3.x * 3 + 1, g_coordinateFramesTextureSize)).xyz;
				zAxisCF[9] = texture(TexCoordinateFrames, textureCoordinates2D(  strandIDs3.x * 3 + 2, g_coordinateFramesTextureSize)).xyz;
				yAxisCF[10] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs3.y * 3 + 1, g_coordinateFramesTextureSize)).xyz;
				zAxisCF[10] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs3.y * 3 + 2, g_coordinateFramesTextureSize)).xyz;
				yAxisCF[11] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs3.z * 3 + 1, g_coordinateFramesTextureSize)).xyz;
				zAxisCF[11] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs3.z * 3 + 2, g_coordinateFramesTextureSize)).xyz;
			}
		}
		else if( position11.w == 1 ||
				 position12.w == 1 ||
				 position13.w == 1)
		{
			// last segment
			uint readPos0 = uint(gl_PrimitiveID - 1);		// 0-1
			vec4 strandIDs0 = readTextureValue(TexIndices, g_textureSizeIndices, readPos0);
			
			// position 0
			position[0] = readTextureValue(Tex0, g_textureSize, uint( strandIDs0.x ) );
			position[1] = readTextureValue(Tex0, g_textureSize, uint( strandIDs0.y ) );
			position[2] = readTextureValue(Tex0, g_textureSize, uint( strandIDs0.z ) );
			// position 1
			position[3] = position11;
			position[4] = position12;
			position[5] = position13;
			// position 2
			position[6] = position[9]  = readTextureValue(Tex0, g_textureSize, uint( strandIDs2.x ) );
			position[7] = position[10] = readTextureValue(Tex0, g_textureSize, uint( strandIDs2.y ) );
			position[8] = position[11] = readTextureValue(Tex0, g_textureSize, uint( strandIDs2.z ) );
			
			if( calculateClumpPosition ) 
			{
				// coordinate frames
				// CF0
				yAxisCF[0] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs0.x * 3 + 1, g_coordinateFramesTextureSize)).xyz;
				zAxisCF[0] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs0.x * 3 + 2, g_coordinateFramesTextureSize)).xyz;
				yAxisCF[1] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs0.y * 3 + 1, g_coordinateFramesTextureSize)).xyz;
				zAxisCF[1] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs0.y * 3 + 2, g_coordinateFramesTextureSize)).xyz;
				yAxisCF[2] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs0.z * 3 + 1, g_coordinateFramesTextureSize)).xyz;
				zAxisCF[2] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs0.z * 3 + 2, g_coordinateFramesTextureSize)).xyz;
				
				// CF1
				yAxisCF[3] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs1.x * 3 + 1, g_coordinateFramesTextureSize)).xyz;
				zAxisCF[3] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs1.x * 3 + 2, g_coordinateFramesTextureSize)).xyz;
				yAxisCF[4] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs1.y * 3 + 1, g_coordinateFramesTextureSize)).xyz;
				zAxisCF[4] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs1.y * 3 + 2, g_coordinateFramesTextureSize)).xyz;
				yAxisCF[5] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs1.z * 3 + 1, g_coordinateFramesTextureSize)).xyz;
				zAxisCF[5] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs1.z * 3 + 2, g_coordinateFramesTextureSize)).xyz;
				
				// CF2
				yAxisCF[6] = yAxisCF[9]  = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs2.x * 3 + 1, g_coordinateFramesTextureSize)).xyz;
				zAxisCF[6] = zAxisCF[9]  = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs2.x * 3 + 2, g_coordinateFramesTextureSize)).xyz;
				yAxisCF[7] = yAxisCF[10] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs2.y * 3 + 1, g_coordinateFramesTextureSize)).xyz;
				zAxisCF[7] = zAxisCF[10] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs2.y * 3 + 2, g_coordinateFramesTextureSize)).xyz;
				yAxisCF[8] = yAxisCF[11] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs2.z * 3 + 1, g_coordinateFramesTextureSize)).xyz;
				zAxisCF[8] = zAxisCF[11] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs2.z * 3 + 2, g_coordinateFramesTextureSize)).xyz;
			}
		}
		else
		{
			uint readPos0 = uint(gl_PrimitiveID - 1);		// 0-1
			vec4 strandIDs0 = readTextureValue(TexIndices, g_textureSizeIndices, readPos0);
			
			uint readPos3 = uint(gl_PrimitiveID + 1);		// 2-3
			vec4 strandIDs3 = readTextureValue(TexIndicesEnd, g_textureSizeIndices, readPos3);
			
			// position 0
			position[0] = readTextureValue(Tex0, g_textureSize, uint( strandIDs0.x ) );
			position[1] = readTextureValue(Tex0, g_textureSize, uint( strandIDs0.y ) );
			position[2] = readTextureValue(Tex0, g_textureSize, uint( strandIDs0.z ) );
			// position 1
			position[3] = position11;
			position[4] = position12;
			position[5] = position13;
			// position 2
			position[6] = readTextureValue(Tex0, g_textureSize, uint( strandIDs2.x ) );
			position[7] = readTextureValue(Tex0, g_textureSize, uint( strandIDs2.y ) );
			position[8] = readTextureValue(Tex0, g_textureSize, uint( strandIDs2.z ) );
			// position 3
			position[9]  = readTextureValue(Tex0, g_textureSize, uint( strandIDs3.x ) );
			position[10] = readTextureValue(Tex0, g_textureSize, uint( strandIDs3.y ) );
			position[11] = readTextureValue(Tex0, g_textureSize, uint( strandIDs3.z ) );
			
			if( calculateClumpPosition ) 
			{
				// coordinate frames
				// CF0
				yAxisCF[0] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs0.x * 3 + 1, g_coordinateFramesTextureSize)).xyz;
				zAxisCF[0] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs0.x * 3 + 2, g_coordinateFramesTextureSize)).xyz;
				yAxisCF[1] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs0.y * 3 + 1, g_coordinateFramesTextureSize)).xyz;
				zAxisCF[1] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs0.y * 3 + 2, g_coordinateFramesTextureSize)).xyz;
				yAxisCF[2] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs0.z * 3 + 1, g_coordinateFramesTextureSize)).xyz;
				zAxisCF[2] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs0.z * 3 + 2, g_coordinateFramesTextureSize)).xyz;
				
				// CF1
				yAxisCF[3] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs1.x * 3 + 1, g_coordinateFramesTextureSize)).xyz;
				zAxisCF[3] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs1.x * 3 + 2, g_coordinateFramesTextureSize)).xyz;
				yAxisCF[4] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs1.y * 3 + 1, g_coordinateFramesTextureSize)).xyz;
				zAxisCF[4] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs1.y * 3 + 2, g_coordinateFramesTextureSize)).xyz;
				yAxisCF[5] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs1.z * 3 + 1, g_coordinateFramesTextureSize)).xyz;
				zAxisCF[5] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs1.z * 3 + 2, g_coordinateFramesTextureSize)).xyz;
				
				// CF2
				yAxisCF[6] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs2.x * 3 + 1, g_coordinateFramesTextureSize)).xyz;
				zAxisCF[6] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs2.x * 3 + 2, g_coordinateFramesTextureSize)).xyz;
				yAxisCF[7] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs2.y * 3 + 1, g_coordinateFramesTextureSize)).xyz;
				zAxisCF[7] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs2.y * 3 + 2, g_coordinateFramesTextureSize)).xyz;
				yAxisCF[8] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs2.z * 3 + 1, g_coordinateFramesTextureSize)).xyz;
				zAxisCF[8] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs2.z * 3 + 2, g_coordinateFramesTextureSize)).xyz;
				
				// CF3
				yAxisCF[9]  = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs3.x * 3 + 1, g_coordinateFramesTextureSize)).xyz;
				zAxisCF[9]  = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs3.x * 3 + 2, g_coordinateFramesTextureSize)).xyz;
				yAxisCF[10] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs3.y * 3 + 1, g_coordinateFramesTextureSize)).xyz;
				zAxisCF[10] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs3.y * 3 + 2, g_coordinateFramesTextureSize)).xyz;
				yAxisCF[11] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs3.z * 3 + 1, g_coordinateFramesTextureSize)).xyz;
				zAxisCF[11] = texture(TexCoordinateFrames, textureCoordinates2D( strandIDs3.z * 3 + 2, g_coordinateFramesTextureSize)).xyz;
			}
		}

	//#ifdef SCREEN_SPACE_ADAPTIVE_TESSELLATION 
		if( g_screenSpaceAdpativeTessellation == 1.0 )
		{
			// Calculate tessellationfactors for screen space adaptive tessellation
			// Get the screen space position of each control point		
			vec2 screenPosition1 = GetScreenSpacePosition(position[3].xyz);
			vec2 screenPosition2 = GetScreenSpacePosition(position[6].xyz);
			vec2 screenPosition3 = GetScreenSpacePosition(position[4].xyz);
			vec2 screenPosition4 = GetScreenSpacePosition(position[7].xyz);
			vec2 screenPosition5 = GetScreenSpacePosition(position[5].xyz);
			vec2 screenPosition6 = GetScreenSpacePosition(position[8].xyz);
			
			float distance1 = distance(screenPosition2, screenPosition1);
			float distance2 = distance(screenPosition4, screenPosition3);
			float distance3 = distance(screenPosition6, screenPosition5);
			
			float maxDistance = max(distance3, max(distance1, distance2) );
			
			gl_TessLevelOuter[1] = g_screenSpaceTessellationScale * maxDistance; 
			
			float bendScale1 = readTextureValue(TexStrandBend, g_textureSize, uint( strandIDs1.x ) ).x;
			float bendScale2 = readTextureValue(TexStrandBend, g_textureSize, uint( strandIDs1.y ) ).x;
			float bendScale3 = readTextureValue(TexStrandBend, g_textureSize, uint( strandIDs1.z ) ).x;
			float bendScale = max( bendScale3, max( bendScale1, bendScale2) );
			
			float detailFactor = gl_TessLevelOuter[1] * bendScale;
			
			gl_TessLevelOuter[1] = max(1.0f, detailFactor);
		}
	//#endif
	}
	
	// put through domain/evalution shader
	gl_out[gl_InvocationID].gl_Position = position[gl_InvocationID];
	hs_yAxisCF[gl_InvocationID] = yAxisCF[gl_InvocationID];
	hs_zAxisCF[gl_InvocationID] = zAxisCF[gl_InvocationID];
}

