#version 400 core

in vec4 vertexPosition;
out vec4 fragColor;

uniform sampler2D Gradients;
uniform sampler2D G1234;
uniform sampler2D G5678;

void main (void)
{
	vec2 uv = vertexPosition.xy / vertexPosition.w * 0.5 + vec2(0.5);
	ivec2 size = textureSize(Gradients, 0);
	float ux = 1.0 / float(size.x);
	float uy = 1.0 / float(size.y);
	
	float gradient = texture2D(Gradients, uv).x;
	vec4 valG1234 = texture2D(G1234, uv);
	vec4 valG5678 = texture2D(G5678, uv);
	
	float G[8];
	
	G[0] = valG1234.x;
	G[1] = valG1234.y;
	G[2] = valG1234.z;
	G[3] = valG1234.w;
	
	G[4] = valG5678.x;
	G[5] = valG5678.y;
	G[6] = valG5678.z;
	G[7] = valG5678.w;
	
	// find index of minimal response
	float max_value = G[0];
	int max_idx = 0;
	for(int i = 1; i < G.length(); ++i)
	{
		if(G[i] > max_value)
		{
			max_value = G[i];
			max_idx = i;
		}
	}
	
	vec4 result;
	if(max_idx == 4)
		result = vec4(gradient,0,0,0);
	else if(max_idx == 5)
		result = vec4(0,gradient,0,0);
	else if(max_idx == 6)
		result = vec4(0,0,gradient,0);
	else if(max_idx == 7)
		result = vec4(0,0,0,gradient);
	else
		result = vec4(0,0,0,0);
		
	fragColor = result;
}
