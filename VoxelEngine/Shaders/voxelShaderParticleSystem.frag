#version 430

uniform sampler2D tex;

in vec4 particleSystemColor;
in vec2 fragTexCoord;

out vec4 fragColor;

void main()
{
	vec4 texColor = texture(tex, fragTexCoord);

	fragColor = particleSystemColor * texColor;
}