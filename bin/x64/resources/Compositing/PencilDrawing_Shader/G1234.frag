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
	
	// G1 
	float sum1 = 0.0;
	for(int i = 0; i < kernel_length; ++i)
	{
		x = uv.x + float(i) * ux;
		y = uv.y + float(i) * uy;
		sum1 += gradient(x,y);
	}
	
	// G2 
	float sum2 = 0.0;
	for(int i = 0; i < kernel_length; ++i)
	{
		x = uv.x;
		y = uv.y + float(i) * uy;
		sum2 += gradient(x,y);
	}
	
	// G3 
	float sum3 = 0.0;
	for(int i = 0; i < kernel_length; ++i)
	{
		x = uv.x - float(i) * ux;
		y = uv.y + float(i) * uy;
		sum3 += gradient(x,y);
	}
	
	// G4
	float sum4 = 0.0; 
	for(int i = 0; i < kernel_length; ++i)
	{
		x = uv.x - float(i) * ux;
		y = uv.y;
		sum4 += gradient(x,y);
	}
	
	fragColor = vec4(sum1,sum2,sum3,sum4);
}
