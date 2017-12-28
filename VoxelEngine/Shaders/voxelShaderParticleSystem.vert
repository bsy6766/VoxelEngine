#version 430

layout(location = 0) in vec3 vert;		// Contains only 1 quad (4 vec3)
layout(location = 1) in vec2 uvVert;	// Contains only 1 quad (4 vec2)
layout(location = 2) in vec2 posVert;
layout(location = 3) in vec4 scaleRotVert;
layout(location = 4) in vec4 colorVert;

uniform mat4 projMat;		// Main camera's projection matrix
uniform mat4 modelMat;		// Particle system's model matrix
uniform float opacity;		// Particle system's opacity. Each particle's opacity is multiplied by this.
uniform vec3 color;			// Particle system's color. Each particle's color is multiplied with this.

out vec2 fragTexCoord;
out vec4 particleSystemColor;

void main()
{
	//vec3 finalVert = vert;

    //scale & rotation matrix
	mat4 scaleRotMat;
    scaleRotMat[0][0] = scaleRotVert[0];
    scaleRotMat[0][1] = scaleRotVert[1];
    scaleRotMat[1][0] = scaleRotVert[2];
    scaleRotMat[1][1] = scaleRotVert[3];
    scaleRotMat[2][2] = 1.0f;
    scaleRotMat[3][3] = 1.0f;

    vec4 finalVert = scaleRotMat * vec4(vert.xyz, 1.0);
	finalVert.x += posVert.x;
	finalVert.y += posVert.y;

	gl_Position = projMat * modelMat * finalVert;

	fragTexCoord = uvVert;

	particleSystemColor = vec4(colorVert.rgb * color.rgb, colorVert.a * opacity);
}