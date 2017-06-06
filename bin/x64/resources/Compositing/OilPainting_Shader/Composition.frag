#version 400 core

in vec4 vertexPosition;
out vec4 fragColor;

uniform sampler2D colors;
uniform sampler2D brush;
uniform sampler2D varnish;
uniform sampler2D canvas;

void main (void)
{
	vec2 uv = vertexPosition.xy / vertexPosition.w * 0.5 + vec2(0.5);
	vec3 colScene = texture(colors, uv).xyz;
	vec3 colBrush = 2.0 * texture(brush, uv).xyz;
	vec3 colVarnish = texture(varnish, uv).xyz;
	vec3 colCanvas = texture(canvas, uv).xyz;
	
	vec3 colComposed = colScene * colBrush + colVarnish;
	
	fragColor = vec4(colComposed * colCanvas.x, 1);
}