#include "FadeTo.h"

// voxel
#include "UIBase.h"

Voxel::UI::FadeTo::FadeTo()
	: Action()
	, opacity(0.0f)
	, target(nullptr)
{}

bool Voxel::UI::FadeTo::init(const float duration, const float opacity)
{
	if (duration < 0.0f || opacity < 0.0f || opacity > 1.0f)
	{
		return false;
	}

	this->duration = duration;
	this->opacity = opacity;

	return true;
}

Voxel::UI::FadeTo * Voxel::UI::FadeTo::create(const float duration, const float opacity)
{
	auto newFadeTo = new FadeTo();
	if (newFadeTo->init(duration, opacity))
	{
		return newFadeTo;
	}
	else
	{
		delete newFadeTo;
		return nullptr;
	}
}

void Voxel::UI::FadeTo::setTarget(TransformNode * target)
{
	if (target)
	{
		this->target = target;
	}
}

void Voxel::UI::FadeTo::update(const float delta)
{
	elapsedTime += delta;
	
	if (elapsedTime >= duration)
	{
		target->setOpacity(opacity);
	}
	else
	{
		const float curOpacity = target->getOpacity();
		target->setOpacity(curOpacity + ((opacity - curOpacity) * (delta / (duration - (elapsedTime - delta)))));
	}
}