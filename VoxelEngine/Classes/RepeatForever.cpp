#include "RepeatForever.h"

// voxel
#include "Action.h"
#include "UIBase.h"

Voxel::UI::RepeatForever::RepeatForever()
	: Action()
	, action(nullptr)
{}

Voxel::UI::RepeatForever::~RepeatForever()
{
	if (action)
	{
		delete action;
	}
}

bool Voxel::UI::RepeatForever::init(Action * action)
{
	if (action)
	{
		this->action = action;
		return true;
	}

	return false;
}

Voxel::UI::RepeatForever * Voxel::UI::RepeatForever::create(Action * action)
{
	auto newRepeat = new RepeatForever();

	if (newRepeat->init(action))
	{
		return newRepeat;
	}
	else
	{
		delete newRepeat;
		return nullptr;
	}
}

bool Voxel::UI::RepeatForever::isDone() const
{
	// repeat forever never ends
	return false;
}

void Voxel::UI::RepeatForever::reset()
{
	if (action)
	{
		action->reset();
	}
}

void Voxel::UI::RepeatForever::setTarget(TransformNode * target)
{
	if (target)
	{
		action->setTarget(target);
	}
}

void Voxel::UI::RepeatForever::update(const float delta)
{
	action->update(delta);

	if (action->isDone())
	{
		float t = action->getExceededTime();

		action->reset();

		action->update(t);
	}
}