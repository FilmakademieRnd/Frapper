#version 400 core

in vec4 vertexPosition;
out vec4 fragColor;

uniform sampler2D rtSmoothV;
uniform sampler2D rtSeeding;

uniform int line_length = 100;
uniform float mag_thresh = 0.01f;

const float oneSixth = 1.0f / 6.0f;

vec2 flow(vec2 pos)
{
	// integration with RK4
	vec2 k1 = texture2D(rtSmoothV, pos).xy;
	vec2 k2 = texture2D(rtSmoothV, pos + 0.5 * k1).xy;
	vec2 k3 = texture2D(rtSmoothV, pos + 0.5 * k2).xy;
	vec2 k4 = texture2D(rtSmoothV, pos + k3).xy;
	return oneSixth * (k1 + 2.0f * k2 + 2.0f * k3 + k4);
}

bool seed(vec2 pos)
{
	return texture2D(rtSeeding, pos).x == 0.0f;
}

bool possibleEdge(vec2 pos)
{	
	return texture2D(rtSeeding, pos).y == 0.0f;
}

void main (void)
{
	vec2 uv = vertexPosition.xy / vertexPosition.w * 0.5f + vec2(0.5);
	
	ivec2 size = textureSize(rtSmoothV, 0);
	float ux = 1.0f / float(size.x);
	float uy = 1.0f / float(size.y);
		
	bool foundSeed = false;
	int steps = 0;
	
	vec2 curPos = uv;
	// forward
	for(int i = 0; i < line_length; ++i)
	{
		if(length(flow(curPos)) < mag_thresh)
			break;
		
		if(seed(curPos))
		{
			foundSeed = true;
			break;
		}
		
		if(!possibleEdge(uv))
			break;
			
		vec2 curFF = flow(curPos);
		
		if(length(curFF) < mag_thresh) // vector field magnitude thresholding
			break;
		
		curPos += vec2(curFF.x * ux, curFF.y * uy);
		++steps;
	}
	
	if(!foundSeed)
	{
		steps = 0;
		curPos = uv;
		// backward
		for(int i = 0; i < line_length; ++i)
		{
			if(length(flow(curPos)) < mag_thresh)
				break;
		
			if(seed(curPos))
			{
				foundSeed = true;
				break;
			}
			
			if(!possibleEdge(uv))
				break;
				
			vec2 curFF = flow(curPos);
			
			if(length(curFF) < mag_thresh) // vector field magnitude thresholding
				break;
			
			curPos -= vec2(curFF.x * ux, curFF.y * uy);
			++steps;
		}
	}	
	
	vec3 c;
	if(foundSeed)
	{
		float darkness = (line_length - steps) / float(line_length);
		float d = 1.0f - darkness;
		c = vec3(d,d,d);
	}
	else
	{
		c = vec3(1,1,1);
	}
		
	fragColor = vec4(c, 1);
}
