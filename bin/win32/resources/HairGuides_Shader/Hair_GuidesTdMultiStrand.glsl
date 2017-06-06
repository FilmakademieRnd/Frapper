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

//#define DEBUG_COLOR

#define PI 3.1415926535897932384626433832795

int findClosestIndex(vec3 coords)
{
	int closestIndex = 0;

	if(coords.y > coords.x)
	{
		if(coords.z > coords.y )
			closestIndex = 2;
		else
			closestIndex = 1;
	}
	else
	{   
		if(coords.z > coords.x )
			closestIndex = 2;
		else
			closestIndex = 0;
	}
	return closestIndex;
}

in gl_PerVertex {
	vec4 gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[];
} gl_in[];

in vec3 hs_yAxisCF[];
in vec3 hs_zAxisCF[];

// per patch inputs
patch in int hs_calculateClumpPosition;
patch in float hs_clumpTransitionStart;
patch in float hs_strandPosition;
patch in float hs_currentNumOfGuideHairSegments;
patch in float hs_currentNumOfGuideHairSegmentsArray[3];

out gl_PerVertex {
	vec4 gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[];
};

out float strandWidth;
out vec3 ds_tangent;
out float ds_strandPosition;

#ifdef DEBUG_COLOR 
out vec4 color;
#endif

// Tessellation Evaluation Shader (Domain Shader in HLSL)
uniform float g_rootRadius;
uniform float g_tipRadius;
uniform float g_clumpWidth;
uniform float g_strandWidth;
uniform float g_strandTaperingStart;
uniform float g_totalStrandVertices;
uniform float g_curlTextureSize;
uniform float g_curlyHairScale;
uniform float g_deviationsTextureSize;
uniform float g_deviationHairScale;
uniform int g_numStrandVariables;
uniform float g_thinning;
uniform float g_clumpTransitionLength;
uniform float g_strandWidthLOD;

uniform sampler2D TexCurlDeviations;
uniform sampler2D TexDeviations;
uniform sampler1D TexStrandLength;
uniform sampler1D TexRandomCircularCoordinates;
uniform sampler1D TexStrandCoordinates;


layout(isolines, fractional_odd_spacing) in;

void main()
{
	uint iMasterStrand = int(gl_TessCoord.y * gl_TessLevelOuter[0] + 0.5);		// Which strand are we working on
	
	float lengthToRoot = hs_currentNumOfGuideHairSegments - hs_strandPosition + gl_TessCoord.x;

	// Multi strand interpolation
	//-----------------------------------------------------------------------------------------
	float coordPosition = float(iMasterStrand & (g_numStrandVariables-1)) / g_numStrandVariables;
	vec4 coords = texture( TexStrandCoordinates, coordPosition);
	
	// transition to single strand
	bool calculateClumpPosition = false;
	int closestIndex = 0;
	if( hs_calculateClumpPosition == 1 )
	{
		calculateClumpPosition = true;
		closestIndex =  findClosestIndex(coords.xyz);
	}

	// uniform cubic b-splines
	mat4 basisMatrix = mat4
    (
        -1/6.0,  3/6.0, -3/6.0,  1/6.0,
         3/6.0, -6/6.0,  3/6.0,  0,
        -3/6.0,  0,      3/6.0,  0,
         1/6.0,  4/6.0,  1/6.0,  0
    );					
	
	float u = gl_TessCoord.x;
	
	// positions
	vec4 basis = basisMatrix * vec4(u * u * u, u * u, u, 1);
	vec4 position = vec4(0, 0, 0, 1.0);
	vec4 clumpVertexPosition = vec4(0, 0, 0, 1.0);

	// coordinate frames
	vec3 yAxisCF = vec3(0, 0, 0);
	vec3 zAxisCF = vec3(0, 0, 0);
	
	//tangents 
    mat3 basisMatrixQuadratic = mat3
    (
        0.5, -1.0, 0.5,
       -1.0,  1.0, 0.0,
        0.5,  0.5, 0.0
    );    
	vec3 basisTangents  = basisMatrixQuadratic * vec3(u * u, u, 1);
	
	vec3 tangent = vec3(0,0,0);

	// multi strand
	vec4 positions[4];

	positions[0].xyz = 		coords.x * gl_in[0].gl_Position.xyz
						+	coords.y * gl_in[1].gl_Position.xyz
						+	coords.z * gl_in[2].gl_Position.xyz;
	positions[0].w = 1.0;
	positions[1].xyz =		coords.x * gl_in[3].gl_Position.xyz
						+	coords.y * gl_in[4].gl_Position.xyz
						+	coords.z * gl_in[5].gl_Position.xyz;
	positions[1].w = 1.0;
	positions[2].xyz = 		coords.x * gl_in[6].gl_Position.xyz
						+	coords.y * gl_in[7].gl_Position.xyz
						+	coords.z * gl_in[8].gl_Position.xyz;
	positions[2].w = 1.0;
	positions[3].xyz =		coords.x * gl_in[9].gl_Position.xyz
						+	coords.y * gl_in[10].gl_Position.xyz
						+	coords.z * gl_in[11].gl_Position.xyz;
	positions[3].w = 1.0;
	
	for (int c = 0; c < 4; ++c)
	{
		position += basis[c] * positions[c];
	}
	
	// tangent calculation
	vec3 tangents[3];
	tangents[0] = positions[1].xyz - positions[0].xyz;
	tangents[1] = positions[2].xyz - positions[1].xyz;
	tangents[2] = positions[3].xyz - positions[2].xyz;
	
	for(int c = 0; c < 3; ++c) 
	{
		tangent += basisTangents[c] * tangents[c];
	}
	
	// single strand
	if( calculateClumpPosition )
	{
		for (int c = 0; c < 4; ++c)
		{
			// coordinate frames
			int indexToRead = closestIndex + (c*3);
			clumpVertexPosition += basis[c] * vec4(gl_in[indexToRead].gl_Position.xyz, 1.0);
			yAxisCF += basis[c] * hs_yAxisCF[indexToRead];
			zAxisCF += basis[c] * hs_zAxisCF[indexToRead];
		}
		clumpVertexPosition.w = 1.0;
		
		// tangent calculation
		vec3 clumpTangents[3];
		clumpTangents[0] = gl_in[closestIndex + (1*3)].gl_Position.xyz - gl_in[closestIndex + (0*3)].gl_Position.xyz;		// 1 0
		clumpTangents[1] = gl_in[closestIndex + (2*3)].gl_Position.xyz - gl_in[closestIndex + (1*3)].gl_Position.xyz;		// 2 1
		clumpTangents[2] = gl_in[closestIndex + (3*3)].gl_Position.xyz - gl_in[closestIndex + (2*3)].gl_Position.xyz;		// 3 2
		
		vec3 clumpTangent = vec3(0,0,0);
		for(int c = 0; c < 3; ++c) 
		{
			clumpTangent += basisTangents[c] * clumpTangents[c];
		}

		// Random circular coordinates
		//-------------------------------------------------
		float randomCircularCoordinatesPosition = float(iMasterStrand & (g_numStrandVariables-1)) / g_numStrandVariables;
		vec2 clumpCoordinates = texture(TexRandomCircularCoordinates, randomCircularCoordinatesPosition).xy;
		
		// current num of guide hair segments - strand position + tessellation factor x
		float clumpLengthToRoot = hs_currentNumOfGuideHairSegmentsArray[closestIndex] - gl_in[3+closestIndex].gl_Position.w + gl_TessCoord.x;
		
		//create the new position for the hair clump vertex
		float radius = g_clumpWidth * ( g_rootRadius*(1-clumpLengthToRoot) + g_tipRadius*clumpLengthToRoot );

		vec3 clumpPosition = 
			clumpVertexPosition.xyz + 
			yAxisCF * clumpCoordinates.x*radius + 
			zAxisCF * clumpCoordinates.y*radius;
		// 1 - ( (EndOfStrandPositionTransition - current Strand Position) / (g_clumpTransitionLength)
		float clumpTransitionLength = hs_currentNumOfGuideHairSegments * g_clumpTransitionLength;
		float clumpWeight = 0.0;
		if( lengthToRoot >= hs_clumpTransitionStart)
		{
			clumpWeight = 1.0;
		}
		else if( lengthToRoot >= hs_clumpTransitionStart - clumpTransitionLength )
		{
			clumpWeight = (lengthToRoot - (hs_clumpTransitionStart - clumpTransitionLength) ) / clumpTransitionLength;
		}
		clumpWeight = clamp(clumpWeight,0,1);
		clumpWeight = sin((clumpWeight-0.5)*PI)*0.5 + 0.5;
		
		float barycentricWeight = 1.0 - clumpWeight;
		
		position.xyz = position.xyz * barycentricWeight + clumpPosition * clumpWeight;
		tangent = tangent * barycentricWeight + clumpTangent* clumpWeight;
	}
	
	// Curly Hair
	uint readPos1 = uint( gl_PrimitiveID * 64 + int(gl_TessCoord.x * 64.0 + 0.5) );
	
	// Calculate position in texture buffer to read 3D vertex position from there
	float colNr1 = ( readPos1 % uint(g_curlTextureSize) ) / g_curlTextureSize;
	float rowNr1 = floor( readPos1 / uint(g_curlTextureSize) ) / g_curlTextureSize;

	// Read the begin and end vertex positions of the line segment from texture
	vec4 curlOffset = texture(TexCurlDeviations, vec2(colNr1, rowNr1));
	position.x += (g_curlyHairScale * curlOffset.x);
	position.z += (g_curlyHairScale * curlOffset.y);
	
	// Deviations
	float tessellatedmasterStrandLength = (g_totalStrandVertices-1)*64.0;
	uint iVertexHair = uint( gl_PrimitiveID * 64.0 + int(gl_TessCoord.x * 64.0) );
	readPos1 = uint( (iMasterStrand & 1023) * tessellatedmasterStrandLength + iVertexHair );
	
	// Calculate position in texture buffer to read 3D vertex position from there
	colNr1 = ( readPos1 % uint(g_deviationsTextureSize) ) / g_deviationsTextureSize;
	rowNr1 = floor( readPos1 / uint(g_deviationsTextureSize) ) / g_deviationsTextureSize;

	// Read the begin and end vertex positions of the line segment from texture
	vec4 deviationOffset = texture(TexDeviations, vec2(colNr1, rowNr1));
	position.x += (g_deviationHairScale * deviationOffset.x);
	position.z += (g_deviationHairScale * deviationOffset.y);
	
	position.w = 1.0;
	gl_Position = position;
	
	ds_tangent = normalize( tangent );
	
	
	// Strand width
	//thinning the hair lengths
	float strandLengthPosition = float(iMasterStrand & (g_numStrandVariables-1)) / g_numStrandVariables;
    float inLengthFrac = texture( TexStrandLength , strandLengthPosition ).r;
    float maxLength = 1.0-g_thinning + g_thinning*inLengthFrac;			// percentage of max length
	float cutOfLength = hs_currentNumOfGuideHairSegments - (maxLength * hs_currentNumOfGuideHairSegments);
	
	// add LOD to strand width
	float tmpStrandWidth = g_strandWidth * g_strandWidthLOD; 
	// Make hair strand thinner form defined position to top
	if( lengthToRoot > (maxLength * hs_currentNumOfGuideHairSegments) )
    {
		strandWidth = 0.0;
	}
	else if( (hs_strandPosition - gl_TessCoord.x) <= (g_strandTaperingStart+cutOfLength) )
	{
		float ignoredPositions = hs_currentNumOfGuideHairSegments - (g_strandTaperingStart+cutOfLength);
		strandWidth = tmpStrandWidth * (maxLength - ( ( hs_currentNumOfGuideHairSegments - ignoredPositions - hs_strandPosition + gl_TessCoord.x ) / (hs_currentNumOfGuideHairSegments - ignoredPositions) ) );
	}
	else
	{
		strandWidth = tmpStrandWidth;
	}
	
	ds_strandPosition = ( hs_currentNumOfGuideHairSegments - hs_strandPosition  + gl_TessCoord.x) / hs_currentNumOfGuideHairSegments;

#ifdef DEBUG_COLOR 
	// Color calculations
	// Show Colors for debugging
	float r, g, b;
	
	// At the test geometry: 16 Patches 0 to 15
	// At Hank geometry: 900 Patches 0 to 899
	r = g = b = smoothstep(0, 899, gl_PrimitiveID); // Show Primitive ID in Colors
	
	// accurate ramps from red = 0 to blue = 1
	// r = |\ . |     g = | /.\ |     b = |   .\ |
	r = 1.0 - (2.0 * r);
	b = clamp((b * 2.0) - 1, 0.0, 1.0);
	g = (g * 2) - (2 * b);
	
	//Show special ID in Pink
	if (gl_PrimitiveID == 0)
	{
		g = 0;
		b = r = 1;
	}

	color = vec4(r, g, b, 1.0);
#endif
}
