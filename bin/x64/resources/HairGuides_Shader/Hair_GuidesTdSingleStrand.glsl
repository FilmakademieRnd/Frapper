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

//assumes x is already normalized, and x stays as it is
void GramSchmidtOrthoNormalize( vec3 x, inout vec3 y, inout vec3 z )
{
	y = y - dot(x,y)*x;
	y = normalize(y);

	z = z - dot(x,z)*x - dot(y,z)*y;
	z = normalize(z);
}

#define EPSILON 0.0001

in gl_PerVertex {
	vec4 gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[];
} gl_in[];

in vec3 hs_tangent[];
in vec3 hs_yAxisCF[];
in vec3 hs_zAxisCF[];
in float hs_CurrentNumOfGuideHairSegments[];

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
uniform float g_strandWidthLOD;

uniform sampler2D TexCurlDeviations;
uniform sampler2D TexDeviations;
uniform sampler1D TexStrandLength;
uniform sampler1D TexRandomCircularCoordinates;


layout(isolines, equal_spacing) in;

void main()
{
	uint iMasterStrand = int(gl_TessCoord.y * gl_TessLevelOuter[0] + 0.5);		// Which strand are we working on

	float strandPosition1 = gl_in[1].gl_Position.w;
	float currentNumOfGuideHairSegments = hs_CurrentNumOfGuideHairSegments[0];
	float lengthToRoot = currentNumOfGuideHairSegments - strandPosition1 + gl_TessCoord.x;
	
	// uniform cubic b-splines
	mat4 basisMatrix = mat4
    (
        -1/6.0,  3/6.0, -3/6.0,  1/6.0,
         3/6.0, -6/6.0,  3/6.0,  0,
        -3/6.0,  0,      3/6.0,  0,
         1/6.0,  4/6.0,  1/6.0,  0
    );
	
	float u = gl_TessCoord.x;
	
	vec4 basis = basisMatrix * vec4(u * u * u, u * u, u, 1);
	
	// positions
    vec4 position = vec4(0, 0, 0, gl_in[0].gl_Position.w);
	// coordinate frames
	vec3 yAxisCF = vec3(0, 0, 0);
	vec3 zAxisCF = vec3(0, 0, 0);
	
    for (int c = 0; c < 4; ++c)
	{	
        position += basis[c] * gl_in[c].gl_Position;
		
        yAxisCF += basis[c] * hs_yAxisCF[c];
        zAxisCF += basis[c] * hs_zAxisCF[c];
	}
	position.w = 1.0;
	
	//GramSchmidtOrthoNormalize( xAxisCF, yAxisCF, zAxisCF );
	
	//tangents 
    mat3 basisMatrixQuadratic = mat3
    (
        0.5, -1.0, 0.5,
       -1.0,  1.0, 0.0,
        0.5,  0.5, 0.0
    );
    
    vec3 basisTangents  = basisMatrixQuadratic * vec3(u * u, u, 1);
    
    vec3 tangent = vec3(0,0,0);
    for(int c = 0; c < 3; ++c) 
    {    
		tangent += basisTangents[c] * hs_tangent[c];
	}
	
	ds_tangent = normalize( tangent );
	
	//GramSchmidtOrthoNormalize( ds_tangent, yAxisCF, zAxisCF );
	
	// Curly Hair
	uint readPos1 = uint( gl_PrimitiveID * 64 + int(gl_TessCoord.x * 64.0 + 0.5) );
	
	// Calculate position in texture buffer to read 3D vertex position from there
	float colNr1 = ( readPos1 % uint(g_curlTextureSize) ) / g_curlTextureSize;
	float rowNr1 = floor( readPos1 / uint(g_curlTextureSize) ) / g_curlTextureSize;

	// Read the begin and end vertex positions of the line segment from texture
	vec4 curlOffset = texture(TexCurlDeviations, vec2(colNr1, rowNr1));
	
	// Deviations
	float tessellatedmasterStrandLength = g_totalStrandVertices*64.0;
	uint iVertexHair = uint( gl_PrimitiveID * 64 + int(gl_TessCoord.x * 64.0 + 0.5) );
	readPos1 = uint( (iMasterStrand & (g_numStrandVariables-1)) * tessellatedmasterStrandLength + iVertexHair );
	
	// Calculate position in texture buffer to read 3D vertex position from there
	colNr1 = ( readPos1 % uint(g_deviationsTextureSize) ) / g_deviationsTextureSize;
	rowNr1 = floor( readPos1 / uint(g_deviationsTextureSize) ) / g_deviationsTextureSize;

	// Read the begin and end vertex positions of the line segment from texture
	vec4 deviationOffset = texture(TexDeviations, vec2(colNr1, rowNr1));

	
	// Random circular coordinates
	//-------------------------------------------------
	float randomCircularCoordinatesPosition = float(iMasterStrand & (g_numStrandVariables-1)) / g_numStrandVariables;
	vec2 clumpCoordinates = texture(TexRandomCircularCoordinates, randomCircularCoordinatesPosition).xy;
	
	vec2 jitter = curlOffset.xy * g_curlyHairScale;
	jitter +=  deviationOffset.xy * g_deviationHairScale;
	
	clumpCoordinates += jitter;
	
	
	//create the new position for the hair clump vertex
	float radius = g_clumpWidth * ( g_rootRadius*(1-lengthToRoot) + g_tipRadius*lengthToRoot );
	vec4 finalPosition;
	finalPosition.xyz = 
		position.xyz + 
		yAxisCF * clumpCoordinates.x*radius + 
		zAxisCF * clumpCoordinates.y*radius;
	
	finalPosition.w = 1.0;

	//-------------------------------------------------
	
	gl_Position = finalPosition;
	
	//thinning the hair lengths
	float strandLengthPosition = float(iMasterStrand & (g_numStrandVariables-1)) / g_numStrandVariables;
    float inLengthFrac = texture( TexStrandLength , strandLengthPosition ).x;
    float maxLength = 1.0-g_thinning + g_thinning*inLengthFrac;			// percentage of max length
	float cutOfLength = currentNumOfGuideHairSegments - (maxLength * currentNumOfGuideHairSegments);

	// add LOD to strand width
	float tmpStrandWidth = g_strandWidth * g_strandWidthLOD; 
	// Make hair strand thinner form defined position to top
	if( lengthToRoot > (maxLength * currentNumOfGuideHairSegments) )
    {
		strandWidth = 0.0;
	}
	else if( (strandPosition1 - gl_TessCoord.x) <= (g_strandTaperingStart+cutOfLength) )
	{
		float ignoredPositions = currentNumOfGuideHairSegments - (g_strandTaperingStart+cutOfLength);
		strandWidth = tmpStrandWidth * (maxLength - ( ( currentNumOfGuideHairSegments - ignoredPositions - strandPosition1 + gl_TessCoord.x ) / (currentNumOfGuideHairSegments - ignoredPositions) ) );
	}
	else
	{
		strandWidth = tmpStrandWidth;
	}
	
	ds_strandPosition = ( currentNumOfGuideHairSegments - strandPosition1  + gl_TessCoord.x) / currentNumOfGuideHairSegments;

#ifdef DEBUG_COLOR 
	// Debug colors
	//------------------------------
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
	//---------------------------------------
#endif
}
