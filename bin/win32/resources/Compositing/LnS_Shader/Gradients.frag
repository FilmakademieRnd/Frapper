#version 400 core

in vec4 vertexPosition;
out vec4 fragColor;

uniform sampler2D rtScene;

float br(float x, float y)
{
	vec4 c = texture2D(rtScene, vec2(x, y));
	return (0.2126 * c.x + 0.7152 * c.y + 0.0722 * c.z);
}

const float p1 = 0.183;

vec2 dxy(float x, float y, float ux, float uy)
{
	vec2 res;
	float v1 = br(x - ux, y + uy);
	float v2 = br(x + ux, y + uy);
	float v3 = br(x - ux, y - uy);
	float v4 = br(x + ux, y - uy);

	res.x = p1 * v1 - p1 * v2
	+ (1.0 - 2.0 * p1) * br(x - ux, y) + (2.0 * p1 - 1.0) * br(x + ux, y)
	+ p1 * v3 - p1 * v4;

	res.y = p1 * v1 + (1.0 - 2.0 * p1) * br(x, y + uy) + p1 * v2
	- p1 * v3 + (2.0 * p1 - 1.0) * br(x, y - uy) - p1 * v4;

	return res;
}

void main (void)
{
	vec2 uv = vertexPosition.xy / vertexPosition.w * 0.5 + vec2(0.5);
	
	ivec2 size = textureSize(rtScene, 0);
	float ux = 1.0 / float(size.x);
	float uy = 1.0 / float(size.y);
	
	vec2 grad = dxy(uv.x, uv.y, ux, uy);

	fragColor = vec4(grad.x, grad.y, 0, 1);
}
