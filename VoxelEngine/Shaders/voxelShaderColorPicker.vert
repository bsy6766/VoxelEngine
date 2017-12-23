#version 430

layout(location = 0) in vec3 vert;
layout(location = 1) in vec4 color;

uniform mat4 projMat;
uniform mat4 modelMat;

uniform vec4 palleteColor;

out vec4 vertColor;

void main()
{
	gl_Position = projMat * modelMat * vec4(vert, 1.0f);

	if(gl_VertexID < 4)
	{
		if(vert.x > 0)
		{
			// right 
			vertColor = palleteColor;
		}
		else
		{
			vertColor = color;
		}
	}
	else
	{
		vertColor = color;
	}
}