#include "Sequence.h"

// voxel
#include "Action.h"
#include "UIBase.h"

// cpp
#include <iostream>

Voxel::UI::Sequence::Sequence()
	: Action()
	, first(nullptr)
	, second(nullptr)
{}

bool Voxel::UI::Sequence::init(Action * action)
{
	if (action)
	{
		first = action;

		this->duration = first->getDuration();

		return true;
	}
	else
	{
		return false;
	}
}

bool Voxel::UI::Sequence::init(Action * first, Action * second)
{
	if (first && second)
	{
		this->first = first;
		this->second = second;

		this->duration = this->first->getDuration() + this->second->getDuration();

		return true;
	}
	else
	{
		return false;
	}
}

bool Voxel::UI::Sequence::init(const std::initializer_list<Action*>& actions)
{
	auto it = actions.begin();
	auto prev = *it;
	auto size = actions.size();
	
	for (unsigned int i = 1; i < (size - 1); ++i)
	{
		it = std::next(it);
		prev = create(prev, *it);
	}

	first = prev;
	second = *std::next(it);

	for (auto action : actions)
	{
		this->duration += action->getDuration();
	}

	return true;
}

Voxel::UI::Sequence::~Sequence()
{
	if (first)
	{
		delete first;
	}

	if (second)
	{
		delete second;
	}
}

Voxel::UI::Sequence * Voxel::UI::Sequence::create(Action * action)
{
	auto newSeq = new Sequence();

	if (newSeq->init(action))
	{
		return newSeq;
	}
	else
	{
		delete newSeq;
		return nullptr;
	}
}

Voxel::UI::Sequence * Voxel::UI::Sequence::create(Action * first, Action * second)
{
	auto newSeq = new Sequence();

	if (newSeq->init(first, second))
	{
		return newSeq;
	}
	else
	{
		delete newSeq;
		return nullptr;
	}
}

Voxel::UI::Sequence * Voxel::UI::Sequence::create(const std::initializer_list<Action*>& actions)
{
	auto size = actions.size();

	if (size == 0)
	{
		return nullptr;
	}
	else
	{
		if (size == 1)
		{
			return create(*actions.begin());
		}
		else if (size == 2)
		{
			return create(*actions.begin(), *(std::prev(actions.end())));
		}
		else
		{
			auto newSeq = new Sequence();

			if (newSeq->init(actions))
			{
				return newSeq;
			}
			else
			{
				delete newSeq;
				return nullptr;
			}
		}
	}
}

bool Voxel::UI::Sequence::isDone() const
{
	return (first ? first->isDone() : true) && (second ? second->isDone() : true);
}

void Voxel::UI::Sequence::setTarget(TransformNode * target)
{
	if (first)
	{
		first->setTarget(target);
	}

	if (second)
	{
		second->setTarget(target);
	}
}

void Voxel::UI::Sequence::update(const float delta)
{
	elapsedTime += delta;

	if (first)
	{
		if (first->isDone())
		{
			if (second->isDone())
			{
				return;
			}
			else
			{
				second->update(delta);
			}
		}
		else
		{
			first->update(delta);

			if (first->isDone())
			{
				if (second)
				{
					auto t = first->getExceededTime();

					second->update(t);
				}
			}
		}
	}
	else
	{
		return;
	}
}
