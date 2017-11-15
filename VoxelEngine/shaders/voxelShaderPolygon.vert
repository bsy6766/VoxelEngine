#version 430

layout(location = 0) in vec3 vert;

uniform mat4 projMat;
uniform mat4 viewMat;
uniform mat4 modelMat;

void main()
{
	gl_Position = projMat * viewMat * modelMat * vec4(vert, 1.0f);
}