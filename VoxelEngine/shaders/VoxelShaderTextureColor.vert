#version 430

layout(location = 0) in vec3 vert;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 uvVert;

uniform mat4 projMat;
uniform mat4 modelMat;
uniform float opacity;

out vec2 fragTexCoord;
out vec4 vertColor;

void main()
{
	gl_Position = projMat * modelMat * vec4(vert, 1);
	vertColor = vec4(color.xyz, opacity);
	fragTexCoord = uvVert;
}