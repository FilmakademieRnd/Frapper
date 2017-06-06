#version 400 core

in vec4 vertexPosition;
out vec4 fragColor;

uniform sampler2D inputMap;

void main (void)
{
	vec2 texCoord = vertexPosition.xy / vertexPosition.w * 0.5 + vec2(0.5);
	vec4 c = texture2D(inputMap, texCoord);
	
	fragColor = c;
}