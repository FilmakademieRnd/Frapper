#version 400 core

in vec4 vertexPosition;
out vec4 fragColor;

uniform sampler2D rtScene;

float br(float xPos, float yPos)
{
	vec4 c = texture2D(rtScene, vec2(xPos, yPos));
	return (0.2126f * c.x + 0.7152f * c.y + 0.0722f * c.z);
}

float gradientX(float x, float y, float ux, float uy)
{
	return (br(x + ux, y - uy) + 2.0f * br(x + ux, y) + br(x + ux, y + uy)
		- br(x - ux, y - uy) - 2.0f * br(x - ux, y) - br(x - ux, y + uy));
}

float gradientY(float x, float y, float ux, float uy)
{
	return (br(x - ux, y + uy) + 2.0f * br(x, y + uy) + br(x + ux, y + uy)
		- br(x - ux, y - uy) - 2.0f * br(x, y - uy) - br(x + ux, y - uy));
}



void main (void)
{
	vec2 texCoord = vertexPosition.xy / vertexPosition.w * 0.5 + vec2(0.5);
	ivec2 size = textureSize(rtScene, 0);
	float unitX = 1.0f / float(size.x);
	float unitY = 1.0f / float(size.y);

	float g_x = gradientX(texCoord.x, texCoord.y, unitX, unitY);
	float g_y = gradientY(texCoord.x, texCoord.y, unitX, unitY);

	vec2 p = vec2(-g_y, g_x);

	vec4 result;
	result.x = p.x;
	result.y = p.y;
	result.z = 0.0f;
	result.w = 1.0f;

	fragColor = result;
}
