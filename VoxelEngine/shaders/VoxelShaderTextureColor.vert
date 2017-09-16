#version 430

layout(location = 0) in vec3 vert;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 uvVert;

uniform mat4 cameraMat;
uniform mat4 worldMat;
uniform mat4 modelMat;

out vec2 fragTexCoord;
out vec4 vertColor;

void main()
{
	gl_Position = cameraMat * worldMat * modelMat * vec4(vert, 1);
	vertColor = color;
	fragTexCoord = uvVert;
}