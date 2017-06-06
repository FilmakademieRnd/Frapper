//Author: Jens Metzner
//Date  : 19.10.2012

#version 400 core

uniform sampler2D in_tex;
uniform sampler2D in_texSilhouette;
uniform sampler2D in_texDepthmap;
uniform sampler2D in_texBackground;

in vec4 VertColor;
in vec4 VertTexture;

// NOISE START
//
// Description : Array and textureless GLSL 2D simplex noise function.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : ijm
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-noise
// 

vec3 mod289(vec3 x) 
{
	return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec2 mod289(vec2 x) 
{
	return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 permute(vec3 x) 
{
	return mod289(((x*34.0)+1.0)*x);
}

float snoise(vec2 v)
{
	const vec4 C = vec4(	0.211324865405187,  // (3.0-sqrt(3.0))/6.0
							0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
							-0.577350269189626,  // -1.0 + 2.0 * C.x
							0.024390243902439); // 1.0 / 41.0
	// First corner
	vec2 i  = floor(v + dot(v, C.yy) );
	vec2 x0 = v -   i + dot(i, C.xx);

	// Other corners
	vec2 i1;
	//i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
	//i1.y = 1.0 - i1.x;
	i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
	// x0 = x0 - 0.0 + 0.0 * C.xx ;
	// x1 = x0 - i1 + 1.0 * C.xx ;
	// x2 = x0 - 1.0 + 2.0 * C.xx ;
	vec4 x12 = x0.xyxy + C.xxzz;
	x12.xy -= i1;

	// Permutations
	i = mod289(i); // Avoid truncation effects in permutation
	vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
	+ i.x + vec3(0.0, i1.x, 1.0 ));

	vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
	m = m*m ;
	m = m*m ;

	// Gradients: 41 points uniformly over a line, mapped onto a diamond.
	// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

	vec3 x = 2.0 * fract(p * C.www) - 1.0;
	vec3 h = abs(x) - 0.5;
	vec3 ox = floor(x + 0.5);
	vec3 a0 = x - ox;

	// Normalise gradients implicitly by scaling m
	// Approximation of: m *= inversesqrt( a0*a0 + h*h );
	m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

	// Compute final noise value at P
	vec3 g;
	g.x  = a0.x  * x0.x  + h.x  * x0.y;
	g.yz = a0.yz * x12.xz + h.yz * x12.yw;
	return 130.0 * dot(m, g);
}

// NOISE END

// BLUR START

vec4 radialBlur(sampler2D tex, vec2 uv, float radius, uint numSamples)
{

	vec2 size = textureSize(tex, 0); 
	float unitX = 1.0 / float(size.x);
	float unitY = 1.0 / float(size.y);

	vec2 pixelSize = vec2(unitX, unitY);

	vec2 poissonDisk[64];
	poissonDisk[0] = vec2(-0.613392, 0.617481);
	poissonDisk[1] = vec2(0.170019, -0.040254);
	poissonDisk[2] = vec2(-0.299417, 0.791925);
	poissonDisk[3] = vec2(0.645680, 0.493210);
	poissonDisk[4] = vec2(-0.651784, 0.717887);
	poissonDisk[5] = vec2(0.421003, 0.027070);
	poissonDisk[6] = vec2(-0.817194, -0.271096);
	poissonDisk[7] = vec2(-0.705374, -0.668203);
	poissonDisk[8] = vec2(0.977050, -0.108615);
	poissonDisk[9] = vec2(0.063326, 0.142369);
	poissonDisk[10] = vec2(0.203528, 0.214331);
	poissonDisk[11] = vec2(-0.667531, 0.326090);
	poissonDisk[12] = vec2(-0.098422, -0.295755);
	poissonDisk[13] = vec2(-0.885922, 0.215369);
	poissonDisk[14] = vec2(0.566637, 0.605213);
	poissonDisk[15] = vec2(0.039766, -0.396100);
	poissonDisk[16] = vec2(0.751946, 0.453352);
	poissonDisk[17] = vec2(0.078707, -0.715323);
	poissonDisk[18] = vec2(-0.075838, -0.529344);
	poissonDisk[19] = vec2(0.724479, -0.580798);
	poissonDisk[20] = vec2(0.222999, -0.215125);
	poissonDisk[21] = vec2(-0.467574, -0.405438);
	poissonDisk[22] = vec2(-0.248268, -0.814753);
	poissonDisk[23] = vec2(0.354411, -0.887570);
	poissonDisk[24] = vec2(0.175817, 0.382366);
	poissonDisk[25] = vec2(0.487472, -0.063082);
	poissonDisk[26] = vec2(-0.084078, 0.898312);
	poissonDisk[27] = vec2(0.488876, -0.783441);
	poissonDisk[28] = vec2(0.470016, 0.217933);
	poissonDisk[29] = vec2(-0.696890, -0.549791);
	poissonDisk[31] = vec2(0.034211, 0.979980);
	poissonDisk[32] = vec2(0.503098, -0.308878);
	poissonDisk[33] = vec2(-0.016205, -0.872921);
	poissonDisk[34] = vec2(0.385784, -0.393902);
	poissonDisk[35] = vec2(-0.146886, -0.859249);
	poissonDisk[36] = vec2(0.643361, 0.164098);
	poissonDisk[37] = vec2(0.634388, -0.049471);
	poissonDisk[38] = vec2(-0.688894, 0.007843);
	poissonDisk[39] = vec2(0.464034, -0.188818);
	poissonDisk[40] = vec2(-0.440840, 0.137486);
	poissonDisk[41] = vec2(0.364483, 0.511704);
	poissonDisk[42] = vec2(0.034028, 0.325968);
	poissonDisk[43] = vec2(0.099094, -0.308023);
	poissonDisk[44] = vec2(0.693960, -0.366253);
	poissonDisk[45] = vec2(0.678884, -0.204688);
	poissonDisk[46] = vec2(0.001801, 0.780328);
	poissonDisk[47] = vec2(0.145177, -0.898984);
	poissonDisk[48] = vec2(0.062655, -0.611866);
	poissonDisk[49] = vec2(0.315226, -0.604297);
	poissonDisk[50] = vec2(-0.780145, 0.486251);
	poissonDisk[51] = vec2(-0.371868, 0.882138);
	poissonDisk[52] = vec2(0.200476, 0.494430);
	poissonDisk[53] = vec2(-0.494552, -0.711051);
	poissonDisk[54] = vec2(0.612476, 0.705252);
	poissonDisk[55] = vec2(-0.578845, -0.768792);
	poissonDisk[56] = vec2(-0.772454, -0.090976);
	poissonDisk[57] = vec2(0.504440, 0.372295);
	poissonDisk[58] = vec2(0.155736, 0.065157);
	poissonDisk[59] = vec2(0.391522, 0.849605);
	poissonDisk[60] = vec2(-0.620106, -0.328104);
	poissonDisk[61] = vec2(0.789239, -0.419965);
	poissonDisk[62] = vec2(-0.545396, 0.538133);
	poissonDisk[63] = vec2(-0.178564, -0.596057);

	vec4 color = texture(tex, uv.xy);

	for(uint i=0; i<numSamples; ++i)
	{
		color += texture(tex, uv.xy + poissonDisk[i] * radius * pixelSize);
	}

	color /= (numSamples+1);

	return color;
}

// BLUR END

// SOBEL BEGIN


vec4 sobel(sampler2D tex)
{
	vec4 finalColor;

	vec2 uv = vec2(VertTexture.x, 1-VertTexture.y);

		float sobel_x[9];

		sobel_x[0] = -1.0; sobel_x[1] = 0.0; sobel_x[2] = 1.0; 
		sobel_x[3] = -2.0; sobel_x[4] = 0.0; sobel_x[5] = 2.0; 
		sobel_x[6] = -1.0; sobel_x[7] = 0.0; sobel_x[8] = 1.0; 


		float sobel_y[9];

		sobel_y[0] = -1.0; sobel_y[1] = -2.0; sobel_y[2] = -1.0; 
		sobel_y[3] =  0.0; sobel_y[4] = 0.0; sobel_y[5] = 0.0; 
		sobel_y[6] =  1.0; sobel_y[7] = 2.0; sobel_y[8] = 1.0; 

		float step_w = 1.0/textureSize(tex, 0).x;
		float step_h = 1.0/textureSize(tex, 0).y;
		
		vec2 offset[9];
		offset[0] = vec2(-step_w, -step_h);
		offset[1] = vec2(0.0, -step_h);
		offset[2] = vec2(step_w, -step_h);

		offset[3] = vec2(-step_w, 0.0);
		offset[4] = vec2(0.0, 0.0);
		offset[5] = vec2(step_w, 0.0);

		offset[6] = vec2(-step_w, step_h);
		offset[7] = vec2(0.0, step_h);
		offset[8] = vec2(step_w, step_h);

		vec4 final_x = vec4(0);
		vec4 final_y;

		for(int i=0; i<9; i++ )
		{
			
			vec4 tmp = texture(tex, uv + offset[i]);
			float w_x = sobel_x[i];
			float w_y = sobel_y[i];

			final_x += w_x*tmp;
			final_y += w_y*tmp;
		}

		vec4 s = sqrt(final_x*final_x + final_y*final_y);

		float p = 100;
		s.x = pow(s.x, p);
		s.y = pow(s.y, p);
		s.z = pow(s.z, p);
		finalColor = vec4((s.xxx+s.yyy+s.zzz)/3, 1);

	return finalColor;	
}

// SOBEL END

// MAIN START

void main()
{
	vec2 coords = vec2(VertTexture.x, 1.0-VertTexture.t);	
	vec2 dim = textureSize(in_tex, 0);
	vec4 color = texture(in_tex,coords);
	vec4 silhouette = texture(in_texSilhouette, coords);
	vec4 depth = texture(in_texDepthmap, coords);
	vec4 background = texture(in_texBackground, coords);

	vec4 sobel = 1.0-clamp(sobel(in_texSilhouette),0.0,0.05);
	
	color = color * sobel;
	
	float noise = 0.1;
	float s = 16;
	for(int i=1; i<8; ++i)
	{
		noise += 1.0/float(i)*(snoise(vec2(coords.x*s*i, coords.y*i*s)));
	}
	noise = abs(noise);
	noise *= 0.1;
	float paper = 1-noise;

	vec3 whitePaper = vec3(0.95,0.95,0.95);
	vec3 colorPaperLow = mix( whitePaper.xyz, color.xyz, background.x );
	vec3 colorPaperHigh = mix( color.xyz, whitePaper.xyz, background.x );
	colorPaperHigh = mix( colorPaperHigh.xyz, whitePaper.xyz, paper );

	vec4 resColor = vec4( colorPaperLow * 0.6 + colorPaperLow * 0.4, color.a);

	float borderWidth = 0.1 * noise;
 
	if(coords.x < borderWidth || coords.x  > 1.0-borderWidth) 
	{
		gl_FragData[0] = vec4( mix( whitePaper.xyz, resColor.xyz, noise ), resColor.a);
	}
	else if(coords.y < borderWidth ||coords.y  > 1.0-borderWidth) 
	{
		gl_FragData[0] = vec4( mix( whitePaper.xyz, resColor.xyz, noise ), resColor.a); 
	}
	else gl_FragData[0] = resColor;
	
}

// MAIN END