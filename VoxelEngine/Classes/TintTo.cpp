#include "TintTo.h"

// voxel
#include "UIBase.h"

Voxel::UI::TintTo::TintTo()
	: Action()
	, color(0.0f)
{}

bool Voxel::UI::TintTo::init(const float duration, const glm::vec3& color)
{
	if (duration < 0.0f)
	{
		return false;
	}

	this->duration = duration;
	this->color = glm::clamp(color, 0.0f, 1.0f);

	return true;
}

Voxel::UI::TintTo * Voxel::UI::TintTo::create(const float duration, const glm::vec3& color)
{
	auto newTintTo = new TintTo();
	if (newTintTo->init(duration, color))
	{
		return newTintTo;
	}
	else
	{
		delete newTintTo;
		return nullptr;
	}
}

void Voxel::UI::TintTo::setTarget(TransformNode * target)
{
	if (RenderNode* rn = dynamic_cast<Voxel::UI::RenderNode*>(target))
	{
		this->target = rn;
	}
}

void Voxel::UI::TintTo::update(const float delta)
{
	elapsedTime += delta;
	//std::cout << "TintTo: " << elapsedTime << " / " << duration << "\n";

	if (elapsedTime >= duration)
	{
		target->setColor(color);
		//std::cout << "TintTo done. opacity: " << opacity << "\n";
	}
	else
	{
		const glm::vec3 curColor = target->getColor();
		//std::cout << "TintTo. opacity: " << newOpacity << "\n";
		target->setColor(curColor + ((color - curColor) * (delta / (duration - (elapsedTime - delta)))));
	}
}