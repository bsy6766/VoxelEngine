#include "RotateTo.h"

// cpp
#include <iostream>

// voxel
#include "UIBase.h"

Voxel::UI::RotateTo::RotateTo()
	: Action()
	, angle(0)
	, target(nullptr)
{}

bool Voxel::UI::RotateTo::init(const float duration, const float angle)
{
	if (duration < 0.0f)
	{
		return false;
	}

	this->duration = duration;
	this->angle = angle;

	//wrapAngle();

	return true;
}

void Voxel::UI::RotateTo::wrapAngle()
{
	if (angle > 360.0f)
	{
		while (angle > 360.0f)
		{
			angle -= 360.0f;
		}
	}
	else if (angle <= -360.0f)
	{
		while (angle <= -360.0f)
		{
			angle += 360.0f;
		}
	}
}

Voxel::UI::RotateTo * Voxel::UI::RotateTo::create(const float duration, const float angle)
{
	auto newRotateTo = new RotateTo();

	if (newRotateTo->init(duration, angle))
	{
		return newRotateTo;
	}
	else
	{
		delete newRotateTo;
		return nullptr;
	}
}

void Voxel::UI::RotateTo::setTarget(TransformNode * target)
{
	if (target)
	{
		this->target = target;
	}
}

void Voxel::UI::RotateTo::update(const float delta)
{
	elapsedTime += delta;
	//std::cout << "RotateTo: " << elapsedTime << " / " << duration << "\n";

	if (elapsedTime >= duration)
	{
		//std::cout << "RotateTo done. angle: " << angle << "\n";
		target->setAngle(angle);
	}
	else
	{
		auto curAngle = target->getAngle();

		target->setAngle(curAngle + ((angle - curAngle) * (delta / (duration - (elapsedTime - delta)))));

		//std::cout << "RotateTo. angle: " << target->getAngle() << "\n";
	}
}
