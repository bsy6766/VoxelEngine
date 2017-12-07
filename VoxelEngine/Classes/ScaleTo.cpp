#include "ScaleTo.h"

// cpp
#include <iostream>

// voxel
#include "UIBase.h"

Voxel::UI::ScaleTo::ScaleTo()
	: Action()
	, scale(0.0f)
	, target(nullptr)
{}

bool Voxel::UI::ScaleTo::init(const float duration, const glm::vec2 & scale)
{
	if (duration < 0.0f)
	{
		return false;
	}

	this->duration = duration;
	this->scale = scale;
	
	return true;
}

Voxel::UI::ScaleTo * Voxel::UI::ScaleTo::create(const float duration, const glm::vec2 & scale)
{
	auto newScaleTo = new ScaleTo();

	if (newScaleTo->init(duration, scale))
	{
		return newScaleTo;
	}
	else
	{
		delete newScaleTo;
		return nullptr;
	}
}

void Voxel::UI::ScaleTo::setTarget(TransformNode * target)
{
	if (target)
	{
		this->target = target;
	}
}

void Voxel::UI::ScaleTo::update(const float delta)
{
	elapsedTime += delta;
	//std::cout << "ScaleTo: " << elapsedTime << " / " << duration << "\n";

	if (elapsedTime >= duration)
	{
		//std::cout << "ScaleTo done. dest: (" << destination.x << ", " << destination.y << ")\n";
		target->setScale(scale);
	}
	else
	{
		auto curScale = target->getScale();

		target->setScale(curScale + ((scale - curScale) * (delta / (duration - (elapsedTime - delta)))));

		//std::cout << "ScaleTo: (" << (curPos + movedDist).x << ", " << (curPos + movedDist).y << ")\n";
	}
}
