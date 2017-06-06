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

/////////////////////////////////////////////////
///////////inputs

SamplerState Sampler
{
};

static int perm[512] = { 	151,160,137,91,90,15,
						131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,
						21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
						35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,
						74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,
						230,220,105,92,41,55,46,245,40,244,102,143,54,65,25,63,161,1,216,
						80,73,209,76,132,187,208,89,18,169,200,196,135,130,116,188,159,86,
						164,100,109,198,173,186,3,64,52,217,226,250,124,123,5,202,38,147,
						118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,
						183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,
						172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,
						218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,
						145,235,249,14,239,107,49,192,214,31,181,199,106,157,184,84,204,176,
						115,121,50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,
						141,128,195,78,66,215,61,156,180,
						
						151,160,137,91,90,15,
						131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,
						21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
						35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,
						74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,
						230,220,105,92,41,55,46,245,40,244,102,143,54,65,25,63,161,1,216,
						80,73,209,76,132,187,208,89,18,169,200,196,135,130,116,188,159,86,
						164,100,109,198,173,186,3,64,52,217,226,250,124,123,5,202,38,147,
						118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,
						183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,
						172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,
						218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,
						145,235,249,14,239,107,49,192,214,31,181,199,106,157,184,84,204,176,
						115,121,50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,
						141,128,195,78,66,215,61,156,180
};

cbuffer inputBuffer : register(b[0]) {
	int count;
	float frequency, lacunarity, gain, offset;
	int octaves;
	int dimensions;
	int process;
}

static uint iteration = 0;

/////////////////////////////////////////////////////
/////structs

struct A2V {
	float4 pos	:POSITION;
};

struct V2G {
	float4 pos	:POSITION;
};

struct G2P {
	float4 pos	:SV_POSITION;
};

struct P2F{
	float4 col	:SV_TARGET;
};

static const float ViewportRes = 512;

/////////////////////////////////////////////////////
//functions

float2 fade(float2 t){
	return t * t * t * (t * (t * 6 - 15) + 10); 
}

float3 fade(float3 t){
	return t * t * t * (t * (t * 6 - 15) + 10); 
}

// grad of 2
float grad(int hash, float x, float y) {
	int		h = hash & 7;       	/* CONVERT LO 2 BITS OF HASH CODE */
	float  	u = h < 4 ? x : y,   	/* INTO 4 GRADIENT DIRECTIONS.   */
			v = h < 4 ? y : x;
	return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

// grad of 3
float grad(int hash, float x, float y, float z) {
	int		h = hash & 15;       	/* CONVERT LO 4 BITS OF HASH CODE */
	float  	u = h < 8 ? x : y,   	/* INTO 12 GRADIENT DIRECTIONS.   */
			v = h < 4 ? y : h==12||h==14 ? x : z;
	return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

// noise 2
float pnoise(float2 p) {
	int2 P = (int2)floor(p) & 255;

	p -= floor(p);

	float2 uv = fade(p);

	int IA = P.x;					//octave randomiser
	int IB = P.x+1;
	for(uint i=0; i<iteration; i++){
		IA = perm[IA];
		IB = perm[IB];
	}
	
	int  A = 	perm[IA]+P.y, 
		 AA = 	perm[A], 
		 AB = 	perm[A+1], /* HASH COORDINATES OF */
		 B = 	perm[IB]+P.y, 
		 BA = 	perm[B], 
		 BB = 	perm[B+1]; /* THE 8 CUBE CORNERS, */
	  
	return 	lerp(	lerp( 	grad(perm[AA  ], p.x  , p.y  ),   					/* AND ADD */
							grad(perm[BA  ], p.x-1, p.y  ), uv.x),        		/* BLENDED */
					lerp(	grad(perm[AB  ], p.x  , p.y-1),         			/* RESULTS */
							grad(perm[BB  ], p.x-1, p.y-1), uv.x), uv.y);     	/* FROM  4 */
}

// fractal sum, range -1.0 - 1.0
float fBm(float2 p)
{
	float freq = frequency, amp = 0.5;
	float sum = 0;	
	for(int i = 0; i < octaves; i++) 
	{
		sum += pnoise(p * freq) * amp;
		freq *= lacunarity;
		amp *= gain;
		iteration++;
	}
	return sum;
}
// fractal abs sum, range 0.0 - 1.0
float turbulence(float2 p)
{
	float sum = 0;
	float freq = frequency, amp = 1.0;
	for(int i = 0; i < octaves; i++) 
	{
		sum += abs(pnoise(p*freq))*amp;
		freq *= lacunarity;
		amp *= gain;
		iteration++;
	}
	return sum;
}
// Ridged multifractal, range 0.0 - 1.0
// See "Texturing & Modeling, A Procedural Approach", Chapter 12
float ridge(float h)
{
    h = abs(h);
    h = offset - h;
    h = h * h;
    return h;
}
float ridgedmf(float2 p)
{
	float sum = 0;
	float freq = frequency, amp = 0.5;
	float prev = 1.0;
	for(int i = 0; i < octaves; i++) 
	{
		float n = ridge(pnoise(p*freq));
		sum += n*amp*prev;
		prev = n;
		freq *= lacunarity;
		amp *= gain;
		iteration++;
	}
	return sum;
}
// noise 3
float pnoise(float3 p) {
	int3	P = (int3)floor(p) & 255;

	 p -= floor(p);

	float3 uvw = fade(p);

	int  A = perm[P.x]+P.y, 
		 AA = perm[A]+P.z, 
		 AB = perm[A+1]+P.z, /* HASH COORDINATES OF */
		 B = perm[P.x+1]+P.y, 
		 BA = perm[B]+P.z, 
		 BB = perm[B+1]+P.z; /* THE 8 CUBE CORNERS, */
	  
	return lerp(	lerp(	lerp( 	grad(perm[AA  ], p.x  , p.y  , p.z  ),   					/* AND ADD */
									grad(perm[BA  ], p.x-1, p.y  , p.z  ), uvw.z),        		/* BLENDED */
							lerp(	grad(perm[AB  ], p.x  , p.y-1, p.z  ),         				/* RESULTS */
									grad(perm[BB  ], p.x-1, p.y-1, p.z  ), uvw.x), uvw.y),     	/* FROM  8 */
					lerp( 	lerp(	grad(perm[AA+1], p.x  , p.y  , p.z-1),						/* CORNERS */
									grad(perm[BA+1], p.x-1, p.y  , p.z-1), uvw.x),      		/* OF CUBE */
							lerp(	grad(perm[AB+1], p.x  , p.y-1, p.z-1),
									grad(perm[BB+1], p.x-1, p.y-1, p.z-1), uvw.z), uvw.y), uvw.z);
}

/////////////////////////////////////////////////////

void main_vs( 	in A2V In,
				out V2G Out	)
{
    Out.pos = In.pos;
}

/////////////////////////////////////////////////////
// creates a flat rectangle over the viewport, no need to change for flatshaders
// also here we calculate the random value array
[maxvertexcount(4)]
void main_gs(	line V2G In[2],
				inout TriangleStream<G2P> Out) {
	
	G2P vA,vB,vC,vD;

	vA.pos = float4( 1, 1, 0, 0);
	vB.pos = float4( 1,-1, 0, 0);
	vC.pos = float4(-1,-1, 0, 0);
	vD.pos = float4(-1, 1, 0, 0);
	Out.Append(vA);
	Out.Append(vB);
	Out.Append(vC);
	Out.Append(vD);
}
/////////////////////////////////////////////////////

void main_ps(	in G2P In,
				out P2F Out	)
{
	
	// set the output
	if(((In.pos.x-.5f)+(In.pos.y-.5f)*ViewportRes)<count){
		float2 p = (float2)(In.pos.xy)/ViewportRes;
		float n = 0;
		if(process<=2) {
			n = fBm(p);
			if(process==1) n = abs(n);
			else if(process==2) n = (n+1)*.5;
		}
		else if(process==3) n = turbulence(p);
		else if(process==4) n = ridgedmf(p);
		
		if(n<=1)
			Out.col = float4(n,n,n,1);
		else Out.col = float4(1,0,0,1);
	} else {
		Out.col = float4(0,0,0,0);
	}
	
	
	
}
