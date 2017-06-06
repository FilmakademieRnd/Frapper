//Author: Jens Metzner
//Date  : 19.10.2012

#version 400 core

uniform sampler2D in_tex;

in vec4 VertColor;
in vec4 VertTexture;

// MAIN START

void main()
{
	vec2 coords = vec2(VertTexture.x, 1.0-VertTexture.t);	
	vec2 dim = textureSize(in_tex, 0);
	vec4 color = texture(in_tex,coords);
	

	gl_FragData[0] = color;
	
}

// MAIN END