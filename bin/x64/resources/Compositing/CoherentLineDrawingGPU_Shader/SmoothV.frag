#version 400 core

in vec4 vertexPosition;
out vec4 fragColor;

uniform sampler2D rtSmoothH;

uniform int SmoothV;

vec2 dirVec(vec2 pos)
{
	return texture2D(rtSmoothH, pos).xy;
}

void main (void)
{
	vec2 ij = vertexPosition.xy / vertexPosition.w * 0.5 + vec2(0.5);
	ivec2 size = textureSize(rtSmoothH, 0);
	vec4 input = texture2D(rtSmoothH, ij);
	float unitY = 1.0f / float(size.y);

	vec2 g = vec2(0.0, 0.0);
	vec2 v = dirVec(ij);

	if(length(v) == 0.0f)
	{
		fragColor = input;
		return;	
	}
	
	for (int s = -SmoothV; s <= SmoothV; ++s)
	{
		vec2 xy = vec2(ij.x, ij.y + float(s) * unitY);
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
