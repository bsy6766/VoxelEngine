#version 430

layout(location = 0) in vec3 vert;
layout(location = 1) in vec3 color;

uniform mat4 cameraMat;
uniform mat4 modelMat;

out vec3 vertColor;

void main()
{
	gl_Position = cameraMat * modelMat * vec4(vert, 1);
	vertColor = color;
}