#version 400 core

in vec2 geomTexture;
out vec4 fragColor;

uniform sampler2D strokeTexture;

void main()
{	
	vec4 c = texture2D(strokeTexture, geomTexture);
	fragColor = vec4(0,0,0,c.x);
	//fragColor = vec4(geomTexture.xy, 0, 1);
}