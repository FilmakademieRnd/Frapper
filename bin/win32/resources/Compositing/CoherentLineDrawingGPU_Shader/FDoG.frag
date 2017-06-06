#version 400 core

in vec4 vertexPosition;
out vec4 fragColor;

uniform sampler2D rtSmoothV;
uniform sampler2D rtDDoG;

uniform float sigma_3 = 3.0f;

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

float getDoGAt(float x, float y)
{
	return texture2D(rtDDoG, vec2(x, y)).x;
}

vec2 getVecAt(vec2 xy)
{
	return texture2D(rtSmoothV, xy).xy;
}

vec2 getVecAt(float x, float y, float ux, float uy)
{
	vec2 texCoord = vec2(x,y);
	vec2 v1 = getVecAt(texCoord);
	vec2 d1 = normalize(v1);
	d1 = vec2(d1.x * ux, d1.y * uy);
	
	vec2 v2 = getVecAt(texCoord + 0.5 * d1);
	vec2 d2 = normalize(v2);
	d2 = vec2(d2.x * ux, d2.y * uy);
	
	vec2 v3 = getVecAt(texCoord + 0.5 * d2).xy;
	vec2 d3 = normalize(v3);
	d3 = vec2(d3.x * ux, d3.y * uy);
	
	vec2 v4 = getVecAt(texCoord + d3);
	return (1.0f / 6.0f) * (v1 + 2.0 * v2 + 2.0 * v3 + v4);
}

vec2 getVecAtBwd(float x, float y, float ux, float uy)
{
	vec2 texCoord = vec2(x,y);
	vec2 v1 = -1.0 * getVecAt(texCoord);
	vec2 d1 = normalize(v1);
	d1 = vec2(d1.x * ux, d1.y * uy);
	
	vec2 v2 = -1.0 * getVecAt(texCoord + 0.5 * d1);
	vec2 d2 = normalize(v2);
	d2 = vec2(d2.x * ux, d2.y * uy);
	
	vec2 v3 = -1.0 * getVecAt(texCoord + 0.5 * d2).xy;
	vec2 d3 = normalize(v3);
	d3 = vec2(d3.x * ux, d3.y * uy);
	
	vec2 v4 = -1.0 * getVecAt(texCoord + d3);
	return (1.0f / 6.0f) * (v1 + 2.0 * v2 + 2.0 * v3 + v4);
}

void main(void)
{
	vec2 texCoord = vertexPosition.xy / vertexPosition.w * 0.5 + vec2(0.5);
	ivec2 size = textureSize(rtSmoothV, 0);

	float unitX = 1.0 / float(size.x);
	float unitY = 1.0 / float(size.y);

	int half_l = getGaussWidth(sigma_3);

	float x, y;
	float val;
	vec2 vt;
	float weight1 = 0.0, w_sum1 = 0.0, sum1 = 0.0;
	
	/////////////// center ///////////////
	val = getDoGAt(texCoord.x, texCoord.y);
	weight1 = gauss(0.0, sigma_3);
	sum1 = val * weight1;
	w_sum1 += weight1;
	
	/////////////// forward ///////////////
	x = texCoord.x;
	y = texCoord.y;
	for (int k = 1; k < half_l; ++k)
	{
		vec2 vt = getVecAt(x, y, unitX, unitY);
		if (vt.x == 0.0 || vt.y == 0.0)
			break;
		val = getDoGAt(x, y);
		weight1 = gauss(float(k), sigma_3);
		sum1 += val * weight1;
		w_sum1 += weight1;
		
		x += vt.x * unitX;
		y += vt.y * unitY;
	}
	
	/////////////// backward ///////////////
	x = texCoord.x;
	y = texCoord.y;
	for (int k = 0; k < half_l; ++k)
	{
		vec2 vt = getVecAtBwd(x, y, unitX, unitY);
		if (vt.x == 0.0 || vt.y == 0.0)
			break;
		val = getDoGAt(x, y);
		weight1 = gauss(float(k), sigma_3);
		sum1 += val * weight1;
		w_sum1 += weight1;

		x += vt.x * unitX;
		y += vt.y * unitY;
	}
	sum1 /= w_sum1;

	float result;
	if (sum1 > 0.0) result = 1.0;
	else result = tanh(sum1);
	
	if(result > 0.0)
		result = 1.0;
	else
		result = 0.0;

	fragColor = vec4(result, result, result, 1);
}
