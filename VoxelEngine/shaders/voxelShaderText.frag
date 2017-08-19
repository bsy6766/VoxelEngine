#version 430

uniform sampler2D tex;

in vec4 vertColor;
in vec2 fragTexCoord;

out vec4 fragColor;

void main()
{
    vec4 textureColor = texture(tex, fragTexCoord);
    /*
    if(textureColor.r > 0)
    {
    	// This pixel is text
		fragColor = vec4(vertColor.rgb, textureColor.r);
    }
    else
    {
    	// this pixel is background
    	vec2 leftCoord = vec2(fragTexCoord.x - 0.01, fragTexCoord.y);
		vec4 leftPixel = texture(tex, leftCoord);
		if(leftPixel.r > 0)
		{
			fragColor = vec4(1, 0, 0, 1);
		}
		else
		{
			fragColor = vec4(0, 0, 0, 0);
		}
    }
    */
	fragColor = vec4(vertColor.rgb, textureColor.r);
}