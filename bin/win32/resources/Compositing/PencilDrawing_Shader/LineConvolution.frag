#version 400 core

in vec4 vertexPosition;
out vec4 fragColor;

uniform sampler2D C1234;
uniform sampler2D C5678;

uniform float KernelRatio = 200.0f;

void main (void)
{
	vec2 uv = vertexPosition.xy / vertexPosition.w * 0.5 + vec2(0.5);
	ivec2 size = textureSize(C1234, 0);
	float ux = 1.0 / float(size.x);
	float uy = 1.0 / float(size.y);
	
	// determine kernel size as proportion of maximum side length
	int kernel_length = int((1.0 / KernelRatio) * float(max(size.x, size.y)));

	float x,y;
	
	// C1
	float sum1 = 0.0;
	for(int i = 0; i < kernel_length; ++i)
	{
		x = uv.x + float(i) * ux;
		y = uv.y + float(i) * uy;
		sum1 += texture2D(C1234, vec2(x,y)).x;
	}
	
	// C2
	float sum2 = 0.0;
	for(int i = 0; i < kernel_length; ++i)
	{
		x = uv.x;
		y = uv.y + float(i) * uy;
		sum2 += texture2D(C1234, vec2(x,y)).y;
	}
	
	// C3
	float sum3 = 0.0;
	for(int i = 0; i < kernel_length; ++i)
	{
		x = uv.x - float(i) * ux;
		y = uv.y + float(i) * uy;
		sum3 += texture2D(C1234, vec2(x,y)).z;
	}
	
	// C4
	float sum4 = 0.0;
	for(int i = 0; i < kernel_length; ++i)
	{
		x = uv.x - float(i) * ux;
		y = uv.y;
		sum4 += texture2D(C1234, vec2(x,y)).w;
	}
	
	// C5
	float sum5 = 0.0;
	for(int i = 0; i < kernel_length; ++i)
	{
		x = uv.x - float(i) * ux;
		y = uv.y - float(i) * uy;
		sum5 += texture2D(C5678, vec2(x,y)).x;
	}
	
	// C6
	float sum6 = 0.0;
	for(int i = 0; i < kernel_length; ++i)
	{
		x = uv.x;
		y = uv.y - float(i) * uy;
		sum6 += texture2D(C5678, vec2(x,y)).y;
	}
	
	// C7
	float sum7 = 0.0;
	for(int i = 0; i < kernel_length; ++i)
	{
		x = uv.x + float(i) * ux;
		y = uv.y - float(i) * uy;
		sum7 += texture2D(C5678, vec2(x,y)).w;
	}
	
	// C8
	float sum8 = 0.0;
	for(int i = 0; i < kernel_length; ++i)
	{
		x = uv.x + float(i) * ux;
		y = uv.y;
		sum8 += texture2D(C5678, vec2(x,y)).w;
	}
	
	float S = sum1 + sum2 + sum3 + sum4 + sum5 + sum6 + sum7 + sum8;
	S = 1.0 - S; // invert for color mapping
	
	fragColor = vec4(S,S,S,1);
}
