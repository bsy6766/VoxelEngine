#version 430

in vec4 vertColor;
in vec4 worldCoord;
in vec4 fragNormal;

out vec4 fragColor;

void main()
{
	fragColor = vertColor;
}