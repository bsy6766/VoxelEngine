#include "ProgressTo.h"

// cpp
#include <iostream>

// voxel
#include "UIBase.h"
#include "ProgressTimer.h"

Voxel::UI::ProgressTo::ProgressTo()
	: Action()
	, percentage(0)
	, target(nullptr)
{}

bool Voxel::UI::ProgressTo::init(const float duration, const float percentage)
{
	if (duration < 0.0f || percentage < 0.0f || percentage > 100.0f)
	{
		return false;
	}

	this->duration = duration;
	this->percentage = static_cast<float>(percentage);

	return true;
}

Voxel::UI::ProgressTo * Voxel::UI::ProgressTo::create(const float duration, const float percentage)
{
	auto newProgressTo = new ProgressTo();

	if (newProgressTo->init(duration, percentage))
	{
		return newProgressTo;
	}
	else
	{
		delete newProgressTo;
		return nullptr;
	}
}

void Voxel::UI::ProgressTo::update(const float delta)
{
	elapsedTime += delta;
	//std::cout << "ProgressTo: " << elapsedTime << " / " << duration << "\n";

	if (elapsedTime >= duration)
	{
		//std::cout << "ProgressTo done. percentage: " << percentage << "\n";
		target->setPercentage(percentage);
	}
	else
	{
		float curPercentage = target->getPercentage();

		target->setPercentage(curPercentage + ((percentage - curPercentage) * (delta / (duration - (elapsedTime - delta)))));

		//std::cout << "ProgressTo. percentage: " << target->getPercentage() << "\n";
	}
}

void Voxel::UI::ProgressTo::setTarget(TransformNode * progressTimer)
{
	if (Voxel::UI::ProgressTimer* pt = dynamic_cast<Voxel::UI::ProgressTimer*>(progressTimer))
	{
		//std::cout << "Adding progress timer as target of progress to\n";
		this->target = pt;
	}
}

