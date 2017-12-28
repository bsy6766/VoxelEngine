#version 430

uniform vec4 lineColor;

in vec4 vertColor;

out vec4 fragColor;

void main()
{
	fragColor = lineColor * vertColor;
}