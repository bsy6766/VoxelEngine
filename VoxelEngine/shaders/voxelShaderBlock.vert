#version 430

layout(location = 0) in vec3 vert;
layout(location = 1) in vec4 color;
layout(location = 2) in vec3 normal;

uniform mat4 projMat;
uniform mat4 viewMat;
uniform mat4 modelMat;

out vec4 vertColor;
out vec4 worldCoord;
out vec4 fragNormal;

void main()
{
	gl_Position = projMat * viewMat * modelMat * vec4(vert, 1);
	worldCoord = modelMat * vec4(vert, 1);
	vertColor = color;
	fragNormal = modelMat * vec4(normal, 1);
}