#version 430

in vec3 vertColor;

out vec4 fragColor;

void main()
{
	fragColor = vec4(vertColor, 1);
}