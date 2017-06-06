#version 400 core

in vec4 vertexPosition;
out vec4 fragColor;

uniform sampler2D Scene;

float br(float x, float y)
{
	vec4 c = texture2D(Scene, vec2(x, y));
	return (0.2126 * c.x + 0.7152 * c.y + 0.0722 * c.z);
}

void main (void)
{
	vec2 uv = vertexPosition.xy / vertexPosition.w * 0.5 + vec2(0.5);
	ivec2 size = textureSize(Scene, 0);
	float ux = 1.0 / float(size.x);
	float uy = 1.0 / float(size.y);
	
	float partial_x = br(uv.x + ux, uv.y) - br(uv.x, uv.y);
	float partial_y = br(uv.x, uv.y + uy) - br(uv.x, uv.y);
	
	float G = sqrt(partial_x * partial_x + partial_y * partial_y);
	
	fragColor = vec4(G,G,G,1);
}
