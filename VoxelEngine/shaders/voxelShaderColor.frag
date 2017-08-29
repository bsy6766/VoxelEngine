#version 430

uniform vec3 playerPosition;
uniform float fogDistance;
uniform vec4 fogColor;
uniform bool fogEnabled;

uniform vec4 ambientColor;
uniform vec3 lightPosition;
uniform float lightIntensity;

in vec4 vertColor;
in vec4 worldCoord;
in vec4 fragNormal;

out vec4 fragColor;

void main()
{
	if(fogEnabled)
	{
		// Everything in side the fog gets affected by light, because it's in render distance. 
		// at least for now.
		vec4 finalLight = vec4(1, 1, 1, 1);

		float lightDistance = abs(distance(lightPosition, worldCoord.xyz));
		vec3 lightVector = normalize(lightPosition - worldCoord.xyz);
		vec3 normalVector = normalize(fragNormal.xyz - worldCoord.xyz);
		float brightness = dot(lightVector, normalVector);
		
		float fadeRatio = 1.0;
		// Check light distance to intensity
		if(lightDistance <= lightIntensity)
		{
			// keep brightness.
			fadeRatio = 1.0;
		}
		else
		{
			// Light fades out if goes further than intensity
			float fadeDist = lightDistance - lightIntensity;
			fadeRatio = 1.0 - (fadeDist / (lightIntensity * 0.5));
			if(fadeRatio < 0) 
			{
				fadeRatio = 0;
			}
		}

		if(fadeRatio == 0)
		{
			brightness = 0;
		}
		else
		{
			brightness = mix(0, brightness, fadeRatio);
		}

		finalLight = clamp((ambientColor + vec4(brightness, brightness, brightness, 1.0)), 0, 1);
	
		float dist = abs(distance(playerPosition, worldCoord.xyz));
		if(dist > fogDistance)
		{
			float fogRatio = (dist - fogDistance) / 16.0f;
			if(fogRatio > 1.0f)
			{
				fogRatio = 1.0f;
			}

			fragColor = mix(vertColor, fogColor, fogRatio) * finalLight;
		}
		else
		{
			fragColor = vertColor * finalLight;
		}
	}
	else
	{
			fragColor = vertColor * ambientColor;
	}
}