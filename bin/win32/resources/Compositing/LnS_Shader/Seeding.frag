#version 400 core

in vec4 vertexPosition;
out vec4 fragColor;

uniform sampler2D rtGradBlurY;

uniform float seed_thresh = 0.95f;
uniform float edge_thresh = 0.1f;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main (void)
{
	vec2 uv = vertexPosition.xy / vertexPosition.w * 0.5 + vec2(0.5);
	
	ivec2 size = textureSize(rtGradBlurY, 0);
	float ux = 1.0 / float(size.x);
	float uy = 1.0 / float(size.y);
	
	vec2 grad = texture2D(rtGradBlurY, uv).xy;

	float G = sqrt(grad.x * grad.x + grad.y * grad.y);
	
	float R;
	if(G > edge_thresh)
		R = 0.0f;
	else
		R = 1.0f;
	
	float rnd = rand(uv);
	float c;
	if(R == 0.0 && rnd > seed_thresh)
		c = 0.0;
	else
		c = 1.0;
		
	vec2 grad2 = texture2D(rtGradBlurY, uv).zw;
	
	float G2 = sqrt(grad2.x * grad2.x + grad2.y * grad2.y);
	
	float s;
	if(G2 > 0.025f)
		s = 0.0;
	else
		s = 1.0;
	
	fragColor = vec4(c, s, 0, 1);
}
