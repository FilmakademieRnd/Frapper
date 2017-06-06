#version 400 core

in vec4 vertexPosition;
out vec4 fragColor;

uniform sampler2D rtGradBlurX;

uniform float sigma2 = 1.0f;
float sigma3 = 3.0f;

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
	ivec2 size = textureSize(rtGradBlurX, 0);

	float uy = 1.0 / float(size.y);

	int w1 = getGaussWidth(sigma2);

	vec2 sum1 = vec2(0.0, 0.0);
	float weights1 = 0.0;

	for(int i = -w1; i <= w1; ++i)
	{
		float weight1 = gauss(float(i), sigma2);
		sum1 += weight1 * texture2D(rtGradBlurX, vec2(texCoord.x, texCoord.y + float(i) * uy)).xy;
		weights1 += weight1;
	}
	sum1 /= weights1;
	
	
	int w2 = getGaussWidth(sigma3);

	vec2 sum2 = vec2(0.0, 0.0);
	float weights2 = 0.0;

	for(int i = -w2; i <= w2; ++i)
	{
		float weight2 = gauss(float(i), sigma3);
		sum2 += weight2 * texture2D(rtGradBlurX, vec2(texCoord.x, texCoord.y + float(i) * uy)).zw;
		weights2 += weight2;
	}
	sum2 /= weights2;

	fragColor = vec4(sum1, sum2);
}
