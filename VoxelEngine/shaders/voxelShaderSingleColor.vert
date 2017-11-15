#version 430

layout(location = 0) in vec3 vert;

uniform mat4 MVPMatrix;

void main()
{
	gl_Position = MVPMatrix * vec4(vert, 1.0f);
}