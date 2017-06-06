//Author: Jens Metzner
//Date  : 19.10.2012

#version 400 core

uniform sampler2D in_tex;
uniform sampler2D in_test;

uniform vec4 id;

in vec4 VertColor;
in vec4 VertTexture;

void segmentation(vec4 a,vec4 b)
{
	if( a.x == b.x && a.y==b.y && a.z==b.z && a.a==b.a)
	{
		gl_FragData[0] = vec4(0,0,0,1);
	}  
	else 
	{
		gl_FragData[0] = vec4(1,1,1,1);
	}
}
void main()
{
	vec2 coords = vec2(VertTexture.x, 1.0-VertTexture.t);	
	vec2 dim = textureSize(in_tex, 0);
	vec4 p = texture(in_tex,coords);
	segmentation(id,p);
}