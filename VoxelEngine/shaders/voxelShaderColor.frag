#version 430

uniform vec3 playerPosition;
uniform float fogDistance;
uniform vec4 fogColor;
uniform bool fogEnabled;

in vec4 vertColor;
in vec4 worldCoord;

out vec4 fragColor;

void main()
{
	if(fogEnabled)
	{
		
		
		float dist = abs(distance(playerPosition, worldCoord.xyz));

		if(dist > fogDistance)
		{
			float fogRatio = (dist - fogDistance) / 16.0f;
			if(fogRatio > 1.0f)
			{
				fogRatio = 1.0f;
			}

			fragColor = mix(vertColor, fogColor, fogRatio);
		}
		else
		{
			fragColor = vertColor;
		}
	}
	else
	{
		fragColor = vertColor;
	}
}