#version 430

uniform vec3 playerPosition;
uniform float fogDistance;
uniform vec4 fogColor;
uniform bool fogEnabled;
uniform float fogLength;

// Directional light. Only 1 directional light exists
// uniform vec3 directionalVector;
// uniform vec3 directionalColor;

// Ambient light
uniform vec4 ambientColor;

// Point light. There can be multiple point light
struct PointLight
{
	vec3 lightPosition;
	vec4 lightColor;
	float lightIntensity;
};

uniform PointLight pointLights[1];

in vec4 vertColor;
in vec4 worldCoord;
in vec4 fragNormal;

out vec4 fragColor;

float getDiffuseBrightness()
{
	float brightness = 0;
		
	float fadeRatio = 1.0;

	float lightDistance = abs(distance(pointLights[0].lightPosition, worldCoord.xyz));

	// Check light distance to intensity
	if(lightDistance <= pointLights[0].lightIntensity)
	{
		// keep brightness.
		fadeRatio = 1.0;
	}
	else
	{
		// Light fades out if goes further than intensity
		float fadeDist = lightDistance - pointLights[0].lightIntensity;
		fadeRatio = 1.0 - (fadeDist / (pointLights[0].lightIntensity * 0.5));
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
		vec3 lightVector = normalize(pointLights[0].lightPosition - worldCoord.xyz);
		vec3 normalVector = normalize(fragNormal.xyz - worldCoord.xyz);
		brightness = dot(lightVector, normalVector);
		brightness = mix(0, brightness, fadeRatio);
	}
	
	clamp(brightness, 0, 1);

	return brightness;
}

void main()
{
	// Get light color
	vec4 finalLight = vec4(1, 1, 1, 1);

	if(pointLights[0].lightIntensity == 0.0)
	{
		finalLight = ambientColor;
	}
	else
	{
		float brightness = getDiffuseBrightness();

		vec4 diffuseColor = pointLights[0].lightColor * vec4(brightness, brightness, brightness, 1.0);
		//diffuseColor.r *= 0.000001;
		//diffuseColor.g *= 0.000001;
		//diffuseColor.b *= 0.000001;
		finalLight = clamp(ambientColor + diffuseColor, 0, 1);
	}

	if(fogEnabled)
	{
		// fog is enabled. Getn distance from player's eye position and pixel in world position
		float dist = abs(distance(playerPosition, worldCoord.xyz));

		// Check distance
		if(dist > fogDistance)
		{
			// Out of fog distance. Fade out color by distance
			float fogRatio = (dist - fogDistance) / fogLength;

			if(fogRatio > 1.0f)
			{
				// clamp
				fogRatio = 1.0f;
			}

			// Mix color with fog color and multiply with light
			fragColor = mix(vertColor * finalLight, fogColor, fogRatio);
		}
		else
		{
			// It's in fog's range. player can see clearly.
			vec4 result = vertColor * finalLight;
			result = clamp(result, vertColor * ambientColor, vec4(1, 1, 1, 1));
			fragColor = result;
		}
	}
	else
	{
			// Fog is diabled. just multiply color with light
			vec4 result = vertColor * finalLight;
			result = clamp(result, vertColor * ambientColor, vec4(1, 1, 1, 1));
			fragColor = result;
	}
}