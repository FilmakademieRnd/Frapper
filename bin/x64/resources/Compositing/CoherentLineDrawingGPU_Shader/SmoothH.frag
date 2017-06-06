#version 400 core

in vec4 vertexPosition;
out vec4 fragColor;

uniform sampler2D rtETF;

uniform int SmoothH;

vec2 dirVec(vec2 pos)
{
	return texture2D(rtETF, pos).xy;
}

void main (void)
{
	vec2 ij = vertexPosition.xy / vertexPosition.w * 0.5 + vec2(0.5);
	ivec2 size = textureSize(rtETF, 0);
	vec4 input = texture2D(rtETF, ij);
	float unitX = 1.0f / float(size.x);
	
	vec2 g = vec2(0.0, 0.0);
	vec2 v = dirVec(ij);
	
	if(length(v) == 0.0f)
	{
		fragColor = input;
		return;	
	}	
	
	for (int s = -SmoothH; s <= SmoothH; ++s)
	{
		vec2 xy = vec2(ij.x + float(s) * unitX, ij.y);
		vec2 w = dirVec(xy);
		
		float mag_diff = length(w) - length(v);
		float angle = dot(v,w);
		float weight = mag_diff + 1.0f;
		g += weight * w * sign(angle);
	}
	g = normalize(g);

	vec4 result;
	result.x = g.x;
	result.y = g.y;
	result.z = 0.0;
	result.w = 1.0;

	fragColor = result;
}
