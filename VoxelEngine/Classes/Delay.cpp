#include "Delay.h"

Voxel::UI::Delay::Delay()
	: Action()
{}

bool Voxel::UI::Delay::init(const float duration)
{
	if (duration < 0.0f)
	{
		return false;
	}

	this->duration = duration;

	return true;
}

Voxel::UI::Delay * Voxel::UI::Delay::create(const float duration)
{
	auto newDelay = new Delay();

	if (newDelay->init(duration))
	{
		return newDelay;
	}
	else
	{
		delete newDelay;
		return false;
	}
}

void Voxel::UI::Delay::update(const float delta)
{
	elapsedTime += delta;
	//std::cout << "delay: " << elapsedTime << " / " << duration << "\n";
}
