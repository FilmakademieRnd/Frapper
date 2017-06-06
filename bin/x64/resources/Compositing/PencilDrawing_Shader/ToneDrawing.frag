#version 400 core

in vec4 vertexPosition;
out vec4 fragColor;

uniform sampler2D Scene;
uniform sampler2D tonal_texture;

const float M_E = 2.718281828459045235;
const float M_PI = 3.141592653589793238;

const float sigma_b = 9.0;

const float u_a = 105.0;
const float u_b = 225.0;

const float my_d = 9.0;
const float sigma_d = 11.0;

uniform float W1 = 11.0f;
uniform float W2 = 37.0f;
uniform float W3 = 52.0f;

float br(float x, float y)
{
	vec4 c = texture2D(Scene, vec2(x, y));
	return (0.2126 * c.x + 0.7152 * c.y + 0.0722 * c.z) * 255.0;
}

float brTonal(float x, float y)
{
	vec4 c = texture2D(tonal_texture, vec2(x, y));
	return (0.2126 * c.x + 0.7152 * c.y + 0.0722 * c.z);
}

float p1(float v)
{
	float result = 0.0;
	if(v <= 1.0)
	{
		float value = (1.0 / sigma_b) * M_E;
		float power = - (1.0 - v) / sigma_b;
		result = pow(value, power);
	}
	
	return result;
}

float p2(float v)
{
	float result = 0.0;
	if(u_a <= v && v <= u_b)
		result = 1.0 / (u_b - u_a);

	return result;
}

float p3(float v)
{
	float value = (1.0 / sqrt(2.0 * M_PI * sigma_d)) * M_E;
	float power = - ((v - my_d) * (v - my_d)) / (2.0 * sigma_d * sigma_d);
	
	return pow(value, power);
}

void main (void)
{
	vec2 uv = vertexPosition.xy / vertexPosition.w * 0.5 + vec2(0.5);
	
	float v = br(uv.x, uv.y);
	float toneP1 = p1(v);
	float toneP2 = p2(v);
	float toneP3 = p3(v);
	
	float T = W1 * toneP1 + W2 * toneP2 + W3 *toneP3;
	T /= 255.0;
	
	const float TONAL_SCALE = 2.0;
	float tonal = brTonal(uv.x / TONAL_SCALE, uv.y / TONAL_SCALE);
	T *= tonal;
	
	fragColor = vec4(T,T,T,1);
}
