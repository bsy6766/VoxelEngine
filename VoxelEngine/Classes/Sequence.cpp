#include "Sequence.h"

// voxel
#include "Action.h"
#include "UIBase.h"

// cpp
#include <iostream>

Voxel::UI::Sequence::Sequence()
	: repeating(false)
	, sequenceState(State::RUNNING)
	, currenActionIndex(-1)
{}

Voxel::UI::Sequence::~Sequence()
{
	for (auto action : actions)
	{
		delete action;
	}
}

bool Voxel::UI::Sequence::init(Action * action, const bool repeat)
{
	if (action)
	{
		actions.push_back(action);

		this->repeating = repeat;
		this->currenActionIndex = 0;

		return true;
	}
	else
	{
		return false;
	}
}

Voxel::UI::Sequence * Voxel::UI::Sequence::create(Action * action, const bool repeat)
{
	auto newSeq = new Sequence();
	
	if (newSeq->init(action, repeat))
	{
		return newSeq;
	}
	else
	{
		delete newSeq;
		return nullptr;
	}
}

bool Voxel::UI::Sequence::init(const std::initializer_list<Action*>& actions, const bool repeat)
{
	if (actions.size() > 0)
	{
		for (auto action : actions)
		{
			if (action == nullptr)
			{
				return false;
			}

			this->actions.push_back(action);
		}

		this->repeating = repeat;
		this->currenActionIndex = 0;

		return true;
	}
	else
	{
		return false;
	}
}

Voxel::UI::Sequence * Voxel::UI::Sequence::create(const std::initializer_list<Action*>& actions, const bool repeat)
{
	auto newSeq = new Sequence();

	if (newSeq->init(actions, repeat))
	{
		return newSeq;
	}
	else
	{
		delete newSeq;
		return nullptr;
	}
}

void Voxel::UI::Sequence::repeat()
{
	repeating = true;
}

bool Voxel::UI::Sequence::isRepeating() const
{
	return repeating;
}

void Voxel::UI::Sequence::stop()
{
	sequenceState = State::STOPPED;
}

bool Voxel::UI::Sequence::isFinished()
{
	return sequenceState == State::FINISHED;
}

void Voxel::UI::Sequence::start()
{
	sequenceState = State::RUNNING;
	this->currenActionIndex = 0;
}

void Voxel::UI::Sequence::pause()
{
	if (sequenceState == State::RUNNING)
	{
		sequenceState = State::PAUSED;
	}
}

void Voxel::UI::Sequence::resume()
{
	if (sequenceState == State::PAUSED)
	{
		sequenceState = State::RUNNING;
	}
}

void Voxel::UI::Sequence::update(const float delta)
{
	if (sequenceState == State::STOPPED)
	{
		return;
	}
	else
	{
		if (sequenceState == State::PAUSED)
		{
			return;
		}
		else
		{
			if (sequenceState == State::RUNNING)
			{
				//std::cout << "seq running. ci = " << currenActionIndex << ", size = " << actions.size() << "\n";
				actions.at(currenActionIndex)->update(delta);

				if (actions.at(currenActionIndex)->isDone())
				{
					currenActionIndex++;
					//std::cout << "next action\n";

					if (currenActionIndex >= (int)actions.size())
					{
						//std::cout << "end if seq\n";
						if (repeating)
						{
							//std::cout << "repeat\n";
							currenActionIndex = 0;

							for (auto action : actions)
							{
								action->reset();
							}

							float exceededTime = actions.at(currenActionIndex)->getExceededTime();
							
							if (exceededTime > 0.0f)
							{
								actions.at(currenActionIndex)->update(delta);
							}
						}
						else
						{
							sequenceState = State::FINISHED;
						}
					}
					else
					{
						float exceededTime = actions.at(currenActionIndex)->getExceededTime();

						if (exceededTime > 0.0f)
						{
							actions.at(currenActionIndex)->update(delta);
						}
					}
				}
			}
		}
	}
}

void Voxel::UI::Sequence::setTarget(TransformNode * target)
{
	for (auto action : actions)
	{
		action->setTarget(target);
	}
}


