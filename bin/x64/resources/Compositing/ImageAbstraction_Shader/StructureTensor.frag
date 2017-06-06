#version 400 core

in vec4 vertexPosition;
out vec4 fragColor;

uniform sampler2D scene;

void main (void)
{
	vec2 uv = vertexPosition.xy / vertexPosition.w * 0.5 + vec2(0.5);
	vec2 src_size = textureSize(scene, 0);
	vec2 d = 1.0 / src_size;
	
    vec3 u = (
           -1.0 * texture(scene, uv + vec2(-d.x, -d.y)).xyz +
           -2.0 * texture(scene, uv + vec2(-d.x,  0.0)).xyz + 
           -1.0 * texture(scene, uv + vec2(-d.x,  d.y)).xyz +
           +1.0 * texture(scene, uv + vec2( d.x, -d.y)).xyz +
           +2.0 * texture(scene, uv + vec2( d.x,  0.0)).xyz + 
           +1.0 * texture(scene, uv + vec2( d.x,  d.y)).xyz
           ) / 4.0;

    vec3 v = (
           -1.0 * texture(scene, uv + vec2(-d.x, -d.y)).xyz + 
           -2.0 * texture(scene, uv + vec2( 0.0, -d.y)).xyz + 
           -1.0 * texture(scene, uv + vec2( d.x, -d.y)).xyz +
           +1.0 * texture(scene, uv + vec2(-d.x,  d.y)).xyz +
           +2.0 * texture(scene, uv + vec2( 0.0,  d.y)).xyz + 
           +1.0 * texture(scene, uv + vec2( d.x,  d.y)).xyz
           ) / 4.0;

    fragColor = vec4(dot(u, u), dot(v, v), dot(u, v), 1.0);
}