#version 400 core

in vec4 vertexPosition;
out vec4 fragColor;

uniform sampler2D rtSmoothH;

uniform float sigma = 10.0f;

#define M_PI 3.14159265358
#define TWO_M_PI 6.28318530718

float gauss(float x, float sigma)
{
	return (exp((-x*x) / (2.0 * sigma*sigma)) / sqrt(TWO_M_PI * sigma * sigma));
}

int getGaussWidth(float sigma)
{
	int i = 0;
	while (true)
	{
		i++;
		if (gauss(float(i), sigma) < 0.01)
			break;
	}
	return i+1;
}

void main (void)
{
	vec2 texCoord = vertexPosition.xy / vertexPosition.w * 0.5 + vec2(0.5);
	ivec2 size = textureSize(rtSmoothH, 0);

	float uy = 1.0 / float(size.y);

	int w = getGaussWidth(sigma);

	vec3 sum = vec3(0.0, 0.0, 0.0);
	float weights = 0.0;

	for(int i = -w; i <= w; ++i)
	{
		float weight = gauss(float(i), sigma);
		sum += weight * texture2D(rtSmoothH, vec2(texCoord.x, texCoord.y + float(i) * uy)).xyz;
		weights += weight;
	}
	sum /= weights;

	fragColor = vec4(sum, 1);
}
