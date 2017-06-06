#version 400 core

in vec4 vertexPosition;
out vec4 fragColor;

uniform sampler2D LineConvolution;
uniform sampler2D ToneDrawing;
uniform sampler2D Scene;

uniform int Monochrome = 0;

const float W_R = 0.299;
const float W_G = 0.587;
const float W_B = 0.114;

vec3 convert_rgb2yuv(vec3 rgb)
{
	vec3 yuv;
	yuv.x = W_R * rgb.x + W_G * rgb.y + W_B * rgb.z;
	yuv.y = 0.492 * (rgb.z - yuv.x);
	yuv.z = 0.877 * (rgb.x - yuv.x);
	return yuv;
}

vec3 convert_yuv2rgb(vec3 yuv)
{
	vec3 rgb;
	rgb.x = yuv.x + yuv.z / 0.877;
	rgb.y = yuv.x - 0.395 * yuv.y - 0.581 * yuv.z;
	rgb.z = yuv.x + yuv.y / 0.492;
	return rgb;
}
    
void main (void)
{
	vec2 uv = vertexPosition.xy / vertexPosition.w * 0.5 + vec2(0.5);

	float line = texture2D(LineConvolution, uv).x;
	float tone = texture2D(ToneDrawing, uv).x;
	
	float R = min(line, tone);
	
	vec3 rgb = vec3(R, R, R);
	
	///// color /////
	if(Monochrome == 0)
	{
		rgb = texture2D(Scene, uv).xyz;
		vec3 yuv = convert_rgb2yuv(rgb);

		yuv.x = R;
		
		rgb = convert_yuv2rgb(yuv);
	}
	///// color /////
	
	fragColor = vec4(rgb, 1.0);
}
