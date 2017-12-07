#include "Action.h"

// voxel
#include "UIBase.h"

// cpp
#include <iostream>
#include "Delay.h"

Voxel::UI::Action::Action()
	: duration(0.0f)
	, elapsedTime(0.0f)
{}

bool Voxel::UI::Action::isDone()
{
	return elapsedTime >= duration;
}

float Voxel::UI::Action::getExceededTime()
{
	if (isDone())
	{
		return elapsedTime - duration;
	}
	else
	{
		return 0.0f;
	}
}

void Voxel::UI::Action::setTarget(TransformNode * target)
{
	// does nothing
}

void Voxel::UI::Action::reset()
{
	elapsedTime = 0.0;
}