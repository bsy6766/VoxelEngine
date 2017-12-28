// pch
#include "PreCompiled.h"

#include "Action.h"

// voxel
#include "BaseNode.h"


Voxel::UI::Action::Action()
	: duration(0.0f)
	, elapsedTime(0.0f)
{
}

bool Voxel::UI::Action::isDone() const
{
	if (duration == 0.0f)
	{
		// instant
		return elapsedTime > 0.0f;
	}
	else
	{
		return elapsedTime >= duration;
	}
}

float Voxel::UI::Action::getExceededTime()
{
	if (elapsedTime >= duration)
	{
		float val = elapsedTime - duration;
		
		elapsedTime = duration;

		return val;
	}
	else
	{
		return 0.0f;
	}
}

float Voxel::UI::Action::getDuration() const
{
	return duration;
}

float Voxel::UI::Action::getElapsedTime() const
{
	return elapsedTime;
}

bool Voxel::UI::Action::isInstant() const
{
	return duration == 0.0f;
}

void Voxel::UI::Action::reset()
{
	elapsedTime = 0.0f;
}

void Voxel::UI::Action::setTarget(TransformNode * target)
{
	// does nothing
}
