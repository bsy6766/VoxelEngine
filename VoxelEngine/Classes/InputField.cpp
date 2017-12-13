#include "InputField.h"

// voxel
#include "UI.h"
#include "UIActions.h"

Voxel::UI::InputField::InputField(const std::string& name)
	: TransformNode(name)
	, text(nullptr)
	, cursor(nullptr)
	, state(State::IDLE)
{}

Voxel::UI::InputField::~InputField()
{
	if (text)
	{
		delete text;
	}

	if (cursor)
	{
		delete cursor;
	}
}

bool Voxel::UI::InputField::init(const std::string & defaultText, const int fontId, const std::string& spriteSheetName, const std::string & cursorImageName)
{
	this->defaultText = defaultText;

	text = Voxel::UI::Text::create(name + "Text", defaultText, fontId);

	if (!text)
	{
		return false;
	}

	cursor = Voxel::UI::Image::create(name + "Cursor", cursorImageName);

	if (!cursor)
	{
		return false;
	}

	cursor->setVisibility(false);
	auto blinkSeq = Voxel::UI::Sequence::create({Voxel::UI::Visibility::create(0.0f, true), Voxel::UI::Visibility::create(0.5f, false), Voxel::UI::Delay::create(0.5f)});

	return true;
}

Voxel::UI::InputField * Voxel::UI::InputField::create(const std::string & name, const std::string & defaultText, const std::string& spriteSheetName, const int fontId, const std::string & cursorImageName)
{
	auto newInputField = new InputField(name);

	if (newInputField->init(defaultText, fontId, cursorImageName))
	{
		return newInputField;
	}
	else
	{
		delete newInputField;
		return nullptr;
	}
}

void Voxel::UI::InputField::setDefaultText(const std::string & defaultText)
{
	this->defaultText = defaultText;
}

std::string Voxel::UI::InputField::getDefaultText() const
{
	return defaultText;
}

void Voxel::UI::InputField::updateModelMatrix()
{
}

void Voxel::UI::InputField::render()
{
	if (visibility)
	{
		auto children_it = children.begin();
		auto beginZOrder = ((children_it)->first).getGlobalZOrder();

		if (beginZOrder < 0)
		{
			// has negative ordered children
			for (; ((children_it)->first).getGlobalZOrder() < 0; children_it++)
			{
				((children_it)->second)->render();
			}
		}
		// else, doesn't have negative ordered children

		// Render self
		

		// Render positive 
		for (; children_it != children.end(); children_it++)
		{
			((children_it)->second)->render();
		}
	}
}
