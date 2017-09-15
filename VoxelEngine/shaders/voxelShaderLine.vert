#version 430

layout(location = 0) in vec3 vert;
layout(location = 1) in vec4 color;

uniform mat4 projMat;
uniform mat4 worldMat;
uniform mat4 modelMat;

out vec4 vertColor;

void main()
{
	vec4 finalPos = projMat * worldMat * modelMat * vec4(vert, 1);
	gl_Position = finalPos;
	vertColor = color;
}