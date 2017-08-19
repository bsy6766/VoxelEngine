#version 430

uniform sampler2D tex;

in vec4 vertColor;
in vec2 fragTexCoord;

out vec4 fragColor;

void main()
{
    vec4 textureColor = texture(tex, fragTexCoord);
	fragColor = vertColor * textureColor;
}