#version 430

layout(location = 0) in vec3 vert;
layout(location = 1) in vec4 color;

uniform mat4 cameraMat;
uniform mat4 modelMat;

out vec4 vertColor;

void main()
{
	gl_Position = cameraMat * modelMat * vec4(vert, 1);
	vertColor = color;
}