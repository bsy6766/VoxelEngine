#version 430

layout(location = 0) in vec3 vert;

uniform mat4 MVP_Matrix;
uniform vec3 topColor;
uniform vec3 bottomColor;

out vec4 vertColor;

void main()
{
	gl_Position =  MVP_Matrix * vec4(vert, 1.0);

	if (vert.y > 0.0)
	{
		vertColor = vec4(topColor, 1.0);
	}
	else
	{
		vertColor = vec4(bottomColor, 1.0);
	}
}