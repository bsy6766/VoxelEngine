#version 430

layout(location = 0) in vec3 vert;		// Contains only 1 quad (4 vec3)
layout(location = 1) in vec2 uvVert;	// Contains only 1 quad (4 vec2)

uniform mat4 projMat;		// Main camera's projection matrix
uniform mat4 modelMat;		// Particle system's model matrix
uniform float opacity;		// Particle system's opacity. Each particle's opacity is multiplied by this.
uniform vec3 color;			// Particle system's color. Each particle's color is multiplied with this.

out vec2 fragTexCoord;
out vec4 particleSystemColor;

void main()
{
	gl_Position = projMat * modelMat * vec4(vert, 1.0);

	fragTexCoord = uvVert;

	particleSystemColor = vec4(color.rgb, opacity);
}