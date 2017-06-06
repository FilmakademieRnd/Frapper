#version 400 core

in vec4 vertex;
out vec4 vertPosition;

void main()
{
	gl_Position = vertex;
	vertPosition = gl_Position;
}