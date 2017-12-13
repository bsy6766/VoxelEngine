#include "Repeat.h"

// voxel
#include "Action.h"
#include "UIBase.h"

Voxel::UI::Repeat::Repeat()
	: Action()
	, action(nullptr)
	, repeat(0)
	, currentRepeat(0)
{}

Voxel::UI::Repeat::~Repeat()
{
	if (action)
	{
		delete action;
	}
}

bool Voxel::UI::Repeat::init(Action * action, const unsigned int repeat)
{
	if (action)
	{
		this->action = action;
		this->repeat = repeat;
	}
	
	return false;
}

Voxel::UI::Repeat * Voxel::UI::Repeat::create(Action * action, const unsigned int repeat)
{
	auto newRepeat = new Repeat();

	if (newRepeat->init(action, repeat))
	{
		return newRepeat;
	}
	else
	{
		delete newRepeat;
		return nullptr;
	}
}

unsigned int Voxel::UI::Repeat::getRepeat() const
{
	return repeat;
}

bool Voxel::UI::Repeat::isDone() const
{
	return (currentRepeat >= repeat && action->isDone());
}

void Voxel::UI::Repeat::reset()
{
	if (action)
	{
		action->reset();
	}
}

void Voxel::UI::Repeat::setTarget(TransformNode * target)
{
	if (target)
	{
		action->setTarget(target);
	}
}

void Voxel::UI::Repeat::update(const float delta)
{
	if (!this->isDone())
	{
		action->update(delta);

		if (action->isDone())
		{
			float t = action->getExceededTime();
			action->reset();

			action->update(t);

			currentRepeat++;
		}
	}
}