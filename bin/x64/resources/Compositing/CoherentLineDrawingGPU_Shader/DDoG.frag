#version 400 core

in vec4 vertexPosition;
out vec4 fragColor;

uniform sampler2D rtScene;
uniform sampler2D rtSmoothV;


uniform float sigma_1 = 1.0f;
uniform float tau;

#define M_PI 3.1415926535897932384626433832795

float gauss(float x, float sigma)
{
	float sigma_sq = sigma * sigma;
	return (exp((-x*x) / (2.0 * sigma_sq)) / sqrt(M_PI * 2.0 * sigma_sq));
}

int getGaussWidth(float sigma)
{
	int i = 0;
	while (true)
	{
		i++;
		if (gauss(float(i), sigma) < 0.001)
			break;
	}
	return i+1;
}

float br(float xPos, float yPos)
{
	vec4 c = texture2D(rtScene, vec2(xPos, yPos));
	return (0.2126 * c.x + 0.7152 * c.y + 0.0722 * c.z);
}

void main(void)
{
	vec2 ij = vertexPosition.xy / vertexPosition.w * 0.5 + vec2(0.5);
	ivec2 size = textureSize(rtSmoothV, 0);
    
	float unitX = 1.0 / float(size.x);
	float unitY = 1.0 / float(size.y);
    
	vec4 currtSmoothV = texture2D(rtSmoothV, ij);
    
	float sigma_2 = 1.6 * sigma_1;

	int half_w1 = getGaussWidth(sigma_1);
	int half_w2 = getGaussWidth(sigma_2);
    
	float x, y;
	int x1, y1;
	float val;
	int dd;
	float weight1 = 0.0, weight2 = 0.0;
	float sum1 = 0.0, sum2 = 0.0;
	float w_sum1 = 0.0, w_sum2 = 0.0;
    
	vec2 vn = vec2(-currtSmoothV.y, currtSmoothV.x);
    
	float result;
	if (vn.x == 0.0 && vn.y == 0.0)
	{
		sum1 = 1.0;
		sum2 = 1.0;
		result = sum1 - tau * sum2;
		fragColor = vec4(result, result, result, 1);
		return;
	}
    
	for (int s = -half_w2; s <= half_w2; s++) {
		x = (ij.x + vn.x * unitX  * float(s)) * float(size.x);
		y = (ij.y + vn.y * unitY * float(s)) * float(size.y);
    
		val = br(x / float(size.x), y / float(size.y));
    
		dd = abs(s);
		if (dd > half_w1) weight1 = 0.0;
		else weight1 = gauss(float(dd), sigma_1);
    
		sum1 += val * weight1;
		w_sum1 += weight1;
    
		weight2 = gauss(float(dd), sigma_2);
		sum2 += val * weight2;
		w_sum2 += weight2;
	}
	sum1 /= w_sum1;
	sum2 /= w_sum2;
    
	result = sum1 - tau * sum2;
		
	fragColor = vec4(result, result, result, 1);
}
