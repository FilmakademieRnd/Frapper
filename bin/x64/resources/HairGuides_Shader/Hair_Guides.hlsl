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

// This allows us to compile the shader with a #define to choose
// the different partition modes for the hull shader.
// See the hull shader: [partitioning(BEZIER_HS_PARTITION)]
// This sample demonstrates "integer", "fractional_even", and "fractional_odd"
#ifndef HS_PARTITION
#define HS_PARTITION "integer"
#endif //HS_PARTITION

// The input patch size.  In this sample, it is 3 vertices(control points).
// This value should match the call to IASetPrimitiveTopology()
#define INPUT_PATCH_SIZE 3

// The output patch size.  In this sample, it is also 3 vertices(control points).
#define OUTPUT_PATCH_SIZE 3

// interpolated attributes for random barycentric coordinates - 1
#define g_NumInterpolatedAttributesMinusOne 1023

//#define SINGLE_STRAND

//--------------------------------------------------------------------------------------
// Constant Buffers
//--------------------------------------------------------------------------------------
cbuffer cbPerFrame : register( b0 )
{
	row_major matrix worldViewProjection;
	row_major matrix inverseWorldMatrix;
	row_major matrix worldMatrix;
	row_major matrix viewProjection;
	float4  cameraPosition;
	float	g_tessellationFactor;
	float	g_maxPatchesPerTessellatedStrand;
	float	g_textureSize;
	float	g_rootRadius;
	float	g_tipRadius;
	float	g_strandWidth;
	float	g_numberOfGuideStrands;
};

//cbuffer cbTessellationBuffer
//{
//	float	g_tessellationFactor;
//	float	g_maxPatchesPerTessellatedStrand;
//	//float	g_textureSize;
//	//float	g_rootRadius;
//};

Texture2D <float4> Tex0 : register( t0 );				// this would also work: Texture2D <float4> Tex0;
Texture1D <float4> g_StrandCoordinates : register( t1 );


//SamplerState g_samLinear : register( s0 );

//SamplerState MySamp;
//SamplerState MySamp
//{
//    Filter = MIN_MAG_POINT_MIP_LINEAR;		// MIN_MAG_MIP_POINT
//    AddressU = Border;
//    AddressV = Border;
//};


//--------------------------------------------------------------------------------------
// Vertex shader section
//--------------------------------------------------------------------------------------
struct VS_CONTROL_POINT_INPUT
{
    float4 vPosition    : POSITION;
	uint instanceID	    : SV_InstanceID;
};

struct VS_CONTROL_POINT_OUTPUT
{
    float4 vPosition        : POSITION;
};

//Just a pass-through shader
VS_CONTROL_POINT_OUTPUT VS( VS_CONTROL_POINT_INPUT Input)
{
	VS_CONTROL_POINT_OUTPUT Output;
	//Output.vPosition = mul( Input.vPosition, worldViewProjection );	// Domain-Shader is doing this transformation
	Output.vPosition = Input.vPosition;
	return Output;
}






//--------------------------------------------------------------------------------------
// Hull Shader section 1
// Constant data function for the HS.  This is executed once per patch.
//--------------------------------------------------------------------------------------
struct HS_CONSTANT_DATA_OUTPUT
{
    float TessFactor[2]          : SV_TessFactor;
};

HS_CONSTANT_DATA_OUTPUT ConstantHS( InputPatch<VS_CONTROL_POINT_OUTPUT, INPUT_PATCH_SIZE> ip,
									uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT Output;

	// Parameter tessellation factor do not work in Release mode
	Output.TessFactor[0] = g_tessellationFactor;
	Output.TessFactor[1] = g_maxPatchesPerTessellatedStrand;

	//Output.TessFactor[0] = 64.0f;
	//Output.TessFactor[1] = 6.0f;


	return Output;
}


//--------------------------------------------------------------------------------------
// Hull Shader section 2
// Main function for the HS.  This is executed for each Control-Point
//--------------------------------------------------------------------------------------

struct HS_OUTPUT
{
    float4 vPosition           : POSITION;  // must be POSITION
};

[domain("isoline")]						//Spline domain as output for our shader
[partitioning(HS_PARTITION)] 			//Partitioning type according to the GUI
[outputtopology("line")]				//Output are line segments
[outputcontrolpoints(INPUT_PATCH_SIZE)]	//Number of times this part of the hull shader will be called for each patch
[patchconstantfunc("ConstantHS")]		//The constant hull shader function

HS_OUTPUT HS( InputPatch<VS_CONTROL_POINT_OUTPUT, INPUT_PATCH_SIZE> p, 
                    uint i : SV_OutputControlPointID,
                    uint PatchID : SV_PrimitiveID)
{
    HS_OUTPUT Output;
    Output.vPosition = p[i].vPosition;
    return Output;
}

//--------------------------------------------------------------------------------------
// Domain Shader Section = Evaluation domain shader
//--------------------------------------------------------------------------------------
struct DS_OUTPUT
{
	float4 vPosition        : SV_Position;	// must be SV_POSITION
	float4 vColor			: COLOR;
	uint readPos			: READPOS;
	float strandWidth		: STRAND_WIDTH;
};

//Domain Shader is invoked for each vertex created by the Tessellator
[domain("isoline")]
DS_OUTPUT DS(	HS_CONSTANT_DATA_OUTPUT input, 
				float2 UV : SV_DomainLocation,
				uint PatchID : SV_PrimitiveID,
				const OutputPatch<HS_OUTPUT, OUTPUT_PATCH_SIZE> cPoint )
{
	DS_OUTPUT Output;

	uint iMasterStrand = (int)(UV.y * input.TessFactor[0] + 0.5);	// Which strand are we working on
	uint iVertexHair = (int)(UV.x * input.TessFactor[1] + 0.5);		//  + g_iSubHairFirstVert;  // Which vertex of the hair are we working on
	
	//uint InstanceID = iMasterStrand; // + input.nHairsBefore;
	//uint vertexID = iVertexHair;
	
	//Baricentric Interpolation to find each position the generated vertices
	//Output.vPosition = (UV.x + 1) * cPoint[0].vPosition;
	
	//float offset = length(finalPos - cPoint[0].vPosition);
	//pos = float3(pos.x + (0.03 * InstanceID), pos.y + sin(offset), pos.z + offset);
	
	//Output.vPosition = mul(float4(pos.xyz, 1), worldViewProjection );
	//Output.vPosition = pos;
	
	// Vertices per strand = segments + end of last segment
	const uint totalStrandVertices = input.TessFactor[1] + 1;

#ifdef SINGLE_STRAND
	// Single strand interpolation
	//-----------------------------------------------------------------------------------------
	//buffer read position calculation for the geometry Shader = vertexID + PatchID * Patch-length;
	uint readPos = (uint) (PatchID * totalStrandVertices + (iVertexHair));
	Output.readPos = readPos;

	// Calculate position in texture buffer to read 3D vertex position from there
	const uint colNr1 = (uint) readPos % (uint) g_textureSize;
	const uint rowNr1 = (uint) floor(readPos / (uint) g_textureSize);
	
	// Read the begin and end vertex positions of the linge segment from texture
	float4 position = Tex0.Load( int3( int( colNr1 ), int( rowNr1 ), 0) );
	position = mul(position, worldMatrix);
	
	// Parameters for the wisp shape
	float r1 = g_rootRadius + g_tipRadius * UV.x;			// UV.x is position in strand
	float f1 = UV.y * 20 + 1;								// UV.y is number of strand
	
	// Order generated hair strands as Archimedean spiral
	Output.vPosition = float4( position.x + f1 * cos(f1) * r1 , position.y , position.z + f1 * sin(f1) * r1 , 1.0);
	


#else
	// Multi strand interpolation
	//-----------------------------------------------------------------------------------------
	// buffer read position calculation for the geometry Shader = vertexID + PatchID * Patch-length;
	// current strand
	//uint readID = PatchID / input.TessFactor[1];
	uint readPos1 = (uint) (PatchID * totalStrandVertices + (iVertexHair));
	Output.readPos = readPos1;
	const uint colNr1 = (uint) readPos1 % (uint) g_textureSize;
	const uint rowNr1 = (uint) floor(readPos1 / (uint) g_textureSize);

	// next strand
	uint readPos2 = (uint) ( (PatchID+1) * (totalStrandVertices) + (iVertexHair));
	uint readPos3 = (uint) ( (PatchID+2) * (totalStrandVertices) + (iVertexHair));
	//if( readPos3 >= (uint) (g_numberOfGuideStrands * totalStrandVertices) )
	//{
	//	readPos1 = (uint) ( (PatchID) * (totalStrandVertices) + (iVertexHair));
	//	readPos2 = (uint) ( (PatchID+1) * (totalStrandVertices) + (iVertexHair));
	//	readPos3 = (uint) ( (0) * (totalStrandVertices) + (iVertexHair));
	//}
	//else if( readPos2 >= (uint) (g_numberOfGuideStrands * totalStrandVertices) )
	//{
	//	readPos1 = (uint) ( (PatchID) * (totalStrandVertices) + (iVertexHair));
	//	readPos2 = (uint) ( (0) * (totalStrandVertices) + (iVertexHair));
	//	readPos3 = (uint) ( (1) * (totalStrandVertices) + (iVertexHair));
	//}

	//else
	//{
	//	readPos2 = (uint) ( (readID+1) * (totalStrandVertices) + (iVertexHair));
	//	readPos3 = (uint) ( (readID+2) * (totalStrandVertices) + (iVertexHair));
	//}

	//if( readPos3 >= (uint) (g_numberOfGuideStrands * totalStrandVertices) )
	//{
	//	readPos1 = (uint) ( (readID-3) * totalStrandVertices + (iVertexHair));
	//	readPos2 = (uint) ( (readID-2) * (totalStrandVertices) + (iVertexHair));
	//	readPos3 = (uint) ( (readID-1) * (totalStrandVertices) + (iVertexHair));
	//}
	//else if( readPos2 >= (uint) (g_numberOfGuideStrands * totalStrandVertices) )
	//{
	//	readPos1 = (uint) ( (readID-2) * totalStrandVertices + (iVertexHair));
	//	readPos2 = (uint) ( (readID-1) * (totalStrandVertices) + (iVertexHair));
	//	readPos3 = (uint) ( (readID) * (totalStrandVertices) + (iVertexHair));
	//}

	//if( ( PatchID + 1 ) >= (uint) (g_numberOfGuideStrands * totalStrandVertices) )
	//{
	//	readPos2 = (uint) ( ( PatchID - 1 * totalStrandVertices ) * (totalStrandVertices) + (iVertexHair));
	//	readPos3 = (uint) ( ( PatchID - 2 * totalStrandVertices ) * (totalStrandVertices) + (iVertexHair));
	//}
	//else if( ( PatchID + 2 ) >= (uint) (g_numberOfGuideStrands * totalStrandVertices) )
	//{
	//	readPos2 = (uint) ( ( PatchID - 1 * totalStrandVertices ) * (totalStrandVertices) + (iVertexHair));
	//	readPos3 = (uint) ( ( PatchID + 1 * totalStrandVertices ) * (totalStrandVertices) + (iVertexHair));
	//}
	//else
	//{
		//readPos2 = (uint) ( ( PatchID + 1 * input.TessFactor[1] ) + (iVertexHair));
		//readPos3 = (uint) ( ( PatchID + 2 * input.TessFactor[1] ) + (iVertexHair));
	//}

	float3x4 masterVertexPosition;
	
	if( readPos2 >= (uint) (g_numberOfGuideStrands * totalStrandVertices) ||
		readPos3 >= (uint) (g_numberOfGuideStrands * totalStrandVertices) )
	{
		masterVertexPosition[0] = float4( 0.0f, 0.0f, 0.0f, 0.0f);
		masterVertexPosition[1] = float4( 0.0f, 0.0f, 0.0f, 0.0f);
		masterVertexPosition[2] = float4( 0.0f, 0.0f, 0.0f, 0.0f);
	}
	else
	{
		const uint colNr2 = (uint) readPos2 % (uint) g_textureSize;
		const uint rowNr2 = (uint) floor(readPos2 / (uint) g_textureSize);
	
		const uint colNr3 = (uint) readPos3 % (uint) g_textureSize;
		const uint rowNr3 = (uint) floor(readPos3 / (uint) g_textureSize);

		masterVertexPosition[0] = Tex0.Load( int3( int( colNr1 ), int( rowNr1 ), 0) );
		masterVertexPosition[1] = Tex0.Load( int3( int( colNr2 ), int( rowNr2 ), 0) );
		masterVertexPosition[2] = Tex0.Load( int3( int( colNr3 ), int( rowNr3 ), 0) );
	}

	//masterVertexPosition[0] = mul(masterVertexPosition[0], worldMatrix);
	//masterVertexPosition[1] = mul(masterVertexPosition[1], worldMatrix);
	//masterVertexPosition[2] = mul(masterVertexPosition[2], worldMatrix);


	// barycentric interpolation
	//float u = UV.x + 1/3;
	//float v = UV.y + 1/3;
	//if( ( u + v ) > 1 )
	//{
	//	u = 1.0f - v;
	//	v = 1.0f - u;
	//}
	//float w = 1.0f - u - v;

	//float u = 1.0f/3.0f * (1 - UV.y);
	//float v = 1.0f/3.0f * UV.y;
	//float w = 1.0f - u - v;
	//Output.vPosition.xyz =		u * masterVertexPosition[0].xyz
	//						+	v * masterVertexPosition[1].xyz
	//						+	w * masterVertexPosition[2].xyz;
	//Output.vPosition.w = 1.0f;

	//float3 coords;
	////coords.xy = g_StrandCoordinates.Load(iMasterStrand & g_NumInterpolatedAttributesMinusOne);
	//coords.xy = g_StrandCoordinates.Load( int2( (int)(iMasterStrand & g_NumInterpolatedAttributesMinusOne), 0));
	//coords.z = (1 - coords.x - coords.y);

	float4 coords = g_StrandCoordinates.Load( int2( (int)(iMasterStrand & g_NumInterpolatedAttributesMinusOne), 0));

	Output.vPosition.xyz =		coords.x * masterVertexPosition[0].xyz
							+	coords.y * masterVertexPosition[1].xyz
							+	coords.z * masterVertexPosition[2].xyz;
	//Output.vPosition.xyz =		(1.0f/3.0f) * masterVertexPosition[0].xyz
	//						+	(1.0f/3.0f) * masterVertexPosition[1].xyz
	//						+	(1.0f/3.0f) * masterVertexPosition[2].xyz;
	Output.vPosition.w = 1.0f;

	Output.vPosition = mul(Output.vPosition, worldMatrix);
	//Output.vPosition = mul(Output.vPosition, worldViewProjection );


	//float3 coords;
	//coords.xy = g_StrandCoordinates.Load(pwi.ID & g_NumInterpolatedAttributesMinusOne);
	//coords.z = (1 - coords.x - coords.y);

	//float3 masterStrandRoots = g_MasterStrandRootIndices.Load(iWisp);
	//int3 vertexIndices;
	//vertexIndices[0] = floor(input.masterStrandRoots.x) + vertexID;
	//vertexIndices[1] = floor(input.masterStrandRoots.y) + vertexID;
	//vertexIndices[2] = floor(input.masterStrandRoots.z) + vertexID;

	////barycentric position---------------------------------------------------------------------
	//float3x4 masterVertexPosition;
	//masterVertexPosition[0] = g_TessellatedMasterStrand.Load( vertexIndices[0] );
	//masterVertexPosition[1] = g_TessellatedMasterStrand.Load( vertexIndices[1] );
	//masterVertexPosition[2] = g_TessellatedMasterStrand.Load( vertexIndices[2] );
	//float3 barycenticPosition = coords.x  * masterVertexPosition[0].xyz
	//	+ coords.y * masterVertexPosition[1].xyz
	//	+ coords.z * masterVertexPosition[2].xyz;

	//Output.vPosition = barycenticPosition;
#endif

	// Strand width
	// Make hair strand thinner from bottom to top (calculate width)
	Output.strandWidth = g_strandWidth * (1 - UV.x);


	// Show Colors for debugging
	float r, g, b;
	
	// At the test geometry: 16 Patches 0 to 15
	// At Hank geometry: 900 Patches 0 to 899
	r = g = b = smoothstep(0, 899, PatchID); // Show Patch ID in Colors
	//r = g = b = smoothstep(0, totalStrandVertices, UV.x * totalStrandVertices);
	//r = g = b = smoothstep(0, 112, readPos);
	
	// accurate ramps from red = 0 to blue = 1
	// r = |\ . |     g = | /.\ |     b = |   .\ |
	r = 1.0 - (2.0 * r);
	b = clamp((b * 2.0) - 1, 0.0, 1.0);
	g = (g * 2) - (2 * b);
	
	//Show special ID in Pink
	if (PatchID == 0)
	{
		g = 0;
		b = r = 1;
	}

	Output.vColor = float4(r, g, b, 1.0);
	
    return Output;
}




//--------------------------------------------------------------------------------------
// Geometry Shader Section
//--------------------------------------------------------------------------------------
struct GS_OUTPUT
{
	float4 vPosition        : SV_Position;	// must be SV_POSITION
	float4 vColor			: COLOR;
};

[maxvertexcount(6)]
void GS( line DS_OUTPUT input[2],
		 inout TriangleStream<GS_OUTPUT> stream)
{
	GS_OUTPUT output;

	float4 pos1 = input[0].vPosition;
	float4 pos2 = input[1].vPosition;
	float width1 = input[0].strandWidth;
	float width2 = input[1].strandWidth;

	// Vector from Camera to bottom Vertex of the line segment
	float3 centerOfSegment = pos1.xyz - pos2.xyz;
	float3 camVec = mul(cameraPosition, inverseWorldMatrix).xyz - centerOfSegment;
	// float3 camVec = cameraPosition.xyz - pos1.xyz;
	
	// Tangent of the line segment
	float3 tangent = pos1.xyz - pos2.xyz;
	
	// Calculate offset direction to control the thickness of the hair
	float3 offsetVec = normalize( cross(camVec, tangent));
	
	// Offset positions to for drawing triangles in world space
	float4 pos11 = float4( pos1.xyz + offsetVec * width1, 1 );
	float4 pos12 = float4( pos1.xyz - offsetVec * width1, 1 );
	
	float4 pos21 = float4( pos2.xyz + offsetVec * width2, 1 );
	float4 pos22 = float4( pos2.xyz - offsetVec * width2, 1 );
	
	// Move the vertex-positions into the final space
	pos11 = mul(pos11, worldViewProjection );
	pos12 = mul(pos12, worldViewProjection );
	
	pos21 = mul(pos21, worldViewProjection );
	pos22 = mul(pos22, worldViewProjection );
	
	
	// //Old Way to make camera-facing quads
	// pos1 = mul(float4(pos1.xyz, 1), worldViewProjection );
	// pos2 = mul(float4(pos2.xyz, 1), worldViewProjection );
	
	// //Offset positions to for drawing triangles in eye space
	// pos11 = float4( pos1.x - width1/2.0 ,pos1.yzw );
	// pos12 = float4( pos1.x + width1/2.0 ,pos1.yzw );
	
	// pos21 = float4( pos2.x - width2/2.0 ,pos2.yzw );
	// pos22 = float4( pos2.x + width2/2.0 ,pos2.yzw );
	
	// Draw two Triangles
	output.vPosition = pos11;
	output.vColor = input[0].vColor;
	output.vColor  = float4(output.vColor.r * (0.0), output.vColor.g * (0.0) , output.vColor.b * (0.0) ,1); // Colors are adjusted with * 0.0 to get a slight gradient per segment for debugging
	stream.Append(output);
	
	output.vPosition = pos12;
	output.vColor = input[0].vColor; 
	output.vColor  = float4(output.vColor.r * (0.33), output.vColor.g * (0.33), output.vColor.b * (0.33) ,1); // Colors are adjusted with * 0.33 to get a slight gradient per segment for debugging
	stream.Append(output);
	
	output.vPosition = pos21;
	output.vColor = input[1].vColor; 
	output.vColor  = float4(output.vColor.r * (0.66), output.vColor.g * (0.66), output.vColor.b * (0.66) ,1); // Colors are adjusted with * 0.66 to get a slight gradient per segment for debugging
	stream.Append(output);
	
	output.vPosition = pos22;
	output.vColor = input[1].vColor; 
	output.vColor  = float4(output.vColor.r * (1.0), output.vColor.g * (1.0), output.vColor.b * (1.0) ,1);	// Colors are adjusted with * 1.0 to get a slight gradient per segment for debugging
	stream.Append(output);
	
	stream.RestartStrip();	
	
	// for ( int i = 0; i < 2; i++ )
	// {
		// colNr = (uint) input[i].readPos % (uint)g_textureSize;
		// rowNr = (uint) floor(input[i].readPos / (uint)g_textureSize);
		
		// pos = input[i].vPosition;
		
		// pos = Tex0.Load( int3(int( colNr ), int( rowNr ), 0) ); //+ float4(0.1,0,0,0);
		
		// // Play with wisp shape
		// float r = g_rootRadius + g_tipRadius * input[i].vPosInStrand;
		// float f = input[i].sNr * 20 + 1;
		// // Order generated hair strands as Archimedean spiral
		// pos = float4( pos.x + f * cos(f) * r , pos.y , pos.z + f * sin(f) * r , 1.0);
	
		// //pos = float4(colNr/10.0,rowNr/10.0,0,1);
		
		// output.vPosition = mul(float4(pos.xyz, 1), worldViewProjection );

	
		// // COLOR
		
		// // Show color read from texture = show positions
		// //output.vColor = Tex0.Load( int3( colNr, rowNr , 0) );
		// // Show input color
		// output.vColor = input[i].vColor; 
		
		// // Show segments (3 Versions) but they need a working color from above
		// //output.vColor = float4(i, 1-i,0,1);
		// //output.vColor  = output.vColor + float4(i/2.0, 1-i/2.0 ,0 ,0);
		// output.vColor  = float4(output.vColor.r * (0.5 + i), output.vColor.g * (0.5 + i) ,output.vColor.b * (0.5 + i) ,0);
		
		// // FINAL
		// stream.Append(output);
	// }
		
	// stream.RestartStrip();
}







//--------------------------------------------------------------------------------------
// Pixel shader section
//--------------------------------------------------------------------------------------

float4 PS( GS_OUTPUT Input ) : SV_TARGET
{
    return Input.vColor;
	//return Tex0.Load( int3(int( 1 ), int( 1 ), 0) );
}