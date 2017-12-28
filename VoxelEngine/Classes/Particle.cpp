// pch
#include "PreCompiled.h"

#include "Particle.h"

int Voxel::UI::Particle::idCounter = 0;

Voxel::UI::Particle::Particle()
	: alive(false)
	, id(-1)
	, pos(0.0f)
	, startColor(0.0f)
	, endColor(0.0f)
	, speed(0.0f)
	, dirVec(0.0f)
	, accelRad(0.0f)
	, accelTan(0.0f)
	, lifeSpan(0.0f)
	, livedTime(0.0f)
	, startSize(0.0f)
	, endSize(0.0f)
	, startAngle(0.0f)
	, endAngle(0.0f)
{}

void Voxel::UI::Particle::normalizePoint(glm::vec2 & radial)
{
	if (pos.x || pos.y)
	{
		float n = (pos.x * pos.x) + (pos.y * pos.y);

		if (n == 1.0f)
		{
			// Already normalized
			return;
		}

		n = glm::sqrt(n);

		if (n != 0.0f)
		{
			n = 1.0f / n;
			radial.x = pos.x * n;
			radial.y = pos.y * n;	
		}
		else
		{
			// Too close to 0 or 0
			return;
		}
	}
}
