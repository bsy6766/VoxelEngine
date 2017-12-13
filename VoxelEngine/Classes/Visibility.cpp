#include "Visibility.h"

// voxel
#include "UI.h"

Voxel::UI::Visibility::Visibility()
	: Action()
	, visibility(false)
	, target(nullptr)
{}

bool Voxel::UI::Visibility::init(const float duration, const bool visibility)
{
	if (duration < 0.0f)
	{
		return false;
	}

	this->duration = duration;
	this->visibility = visibility;

	return true;
}

Voxel::UI::Visibility * Voxel::UI::Visibility::create(const float duration, const bool visibility)
{
	auto newVisibility = new Visibility();
	if (newVisibility->init(duration, visibility))
	{
		return newVisibility;
	}
	else
	{
		delete newVisibility;
		return nullptr;
	}
}

void Voxel::UI::Visibility::setTarget(TransformNode * target)
{
	if (target)
	{
		this->target = target;
	}
}

void Voxel::UI::Visibility::update(const float delta)
{
	elapsedTime += delta;

	if (elapsedTime >= duration)
	{
		target->setVisibility(visibility);
	}
}


