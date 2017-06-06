#version 400 core

in vec4 vertexPosition;
out vec4 fragColor;

uniform sampler2D Gradients;

uniform float KernelRatio = 200.0f;

float gradient(float x, float y)
{
	return texture2D(Gradients, vec2(x,y)).x;
}

void main (void)
{
	vec2 uv = vertexPosition.xy / vertexPosition.w * 0.5 + vec2(0.5);
	ivec2 size = textureSize(Gradients, 0);
	float ux = 1.0 / float(size.x);
	float uy = 1.0 / float(size.y);
	
	// determine kernel size as proportion of maximum side length
	int kernel_length = int((1.0 / KernelRatio) * float(max(size.x, size.y)));	
	
	float x,y;
	
	// G5
	float sum5 = 0.0;
	for(int i = 0; i < kernel_length; ++i)
	{
		x = uv.x - float(i) * ux;
		y = uv.y - float(i) * uy;
		sum5 += gradient(x,y);
	}
	
	// G6
	float sum6 = 0.0;
	for(int i = 0; i < kernel_length; ++i)
	{
		x = uv.x;
		y = uv.y - float(i) * uy;
		sum6 += gradient(x,y);
	}
	
	// G7
	float sum7 = 0.0;
	for(int i = 0; i < kernel_length; ++i)
	{
		x = uv.x + float(i) * ux;
		y = uv.y - float(i) * uy;
		sum7 += gradient(x,y);
	}
	
	// G8 
	float sum8 = 0.0;
	for(int i = 0; i < kernel_length; ++i)
	{
		x = uv.x + float(i) * ux;
		y = uv.y;
		sum8 += gradient(x,y);
	}
	
	fragColor = vec4(sum5,sum6,sum7,sum8);
}
