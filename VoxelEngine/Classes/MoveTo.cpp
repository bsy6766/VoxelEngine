#include "MoveTo.h"

// voxel
#include "TransformNode.h"

Voxel::UI::MoveTo::MoveTo()
	: Action()
	, destination(0.0f)
	, target(nullptr)
{}

bool Voxel::UI::MoveTo::init(const float duration, const glm::vec2 & destination)
{
	if (duration < 0.0f)
	{
		return false;
	}

	this->duration = duration;
	this->destination = destination;

	return true;
}

Voxel::UI::MoveTo * Voxel::UI::MoveTo::create(const float duration, const glm::vec2 & destination)
{
	auto newMoveTo = new MoveTo();

	if (newMoveTo->init(duration, destination))
	{
		return newMoveTo;
	}
	else
	{
		delete newMoveTo;
		return nullptr;
	}
}

void Voxel::UI::MoveTo::setTarget(TransformNode * target)
{
	if (target)
	{
		this->target = target;
	}
}

void Voxel::UI::MoveTo::update(const float delta)
{
	elapsedTime += delta;

	if (elapsedTime >= duration)
	{
		target->setPosition(destination);
	}
	else
	{
		auto curPos = target->getPosition();

		target->setPosition(curPos + ((destination - curPos) * (delta / (duration - (elapsedTime - delta)))));
	}
}
