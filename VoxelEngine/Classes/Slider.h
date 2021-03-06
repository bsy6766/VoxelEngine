#ifndef SLIDER_H
#define SLIDER_H

// voxel
#include "RenderNode.h"

// cpp
#include <functional>
#include <array>

namespace Voxel
{
	namespace UI
	{
		/**
		*	@class Slider
		*	@biref Simple slider. Can be vertical or horizontal.
		*/
		class Slider : public RenderNode
		{
		public:
			enum class State
			{
				IDLE = 0,
				BUTTON_HOVERED,
				BUTTON_CLICKED,
				BUTTON_DISABLED,
				BAR_HOVERED,
				BAR_CLICKED,
			};

			enum class Type
			{
				VERTICAL = 0,
				HORIZONTAL
			};
		private:
			// Constructor
			Slider() = delete;
			Slider(const std::string& name);
			
			// Value reversed
			bool reversedValue;

			// state
			State state;

			// type
			Type type;

			// button 
			GLuint buttonVao;

			// bar
			GLuint barVao;

			// button size
			glm::vec2 buttonSize;
			Voxel::Shape::Rect buttonBoundingBox;
						
			// bar size
			glm::vec2 barSize;
			Voxel::Shape::Rect barBoundingBox;

			// button indices
			unsigned int buttonIndexOffset;

			// value
			float minValue;
			float maxValue;
			float currentValue;

			// On slider button pressed
			std::function<void(Voxel::UI::Slider*)> onButtonPressed;
			// On slider bar pressed
			std::function<void(Voxel::UI::Slider*)> onBarPressed;
			// On slider move
			std::function<void(Voxel::UI::Slider*)> onValueChange;
			// On slider move is finished
			std::function<void(Voxel::UI::Slider*)> onFinished;

			// initilaize
			bool init(const std::string& spriteSheetName, const std::string& barImageName, const std::string& buttonImageName, const Type type, const float minValue = 0, const float maxValue = 100);

			// build bar
			void loadBarBuffer(const std::array<float, 12>& vertices, const std::array<float, 8>& uvs, const std::array<unsigned int, 6>& indicies);

			// build button
			void loadButtonBuffer(const std::vector<float>& vertices, const std::vector<float>& uvs, const std::vector<unsigned int>& indicies);

			// update button indices
			void updateButtonIndexOffset();

			// get value on mouse position
			float getValueOnMousePosition(const glm::vec2& mousePosition);

			// Update button pos
			void updateButtonPos();
		public:
			// Destructor
			~Slider();

			/**
			*	Create slider
			*	@param name Name of slider
			*	@param spriteSheetName Name of spritesheet that has slider images
			*	@param barImageName Name of slider bar image
			*	@param buttonImageName Name of slider button.
			*	@param type Type of slider
			*	@param minValue Minimum value of slider
			*	@param maxValue Maximum value of slider
			*/
			static Slider* create(const std::string& name, const std::string& spriteSheetName, const std::string& barImageName, const std::string& buttonImageName, const Type type, const float minValue = 0, const float maxValue = 100);

			// enables
			void enable();

			// disable 
			void disable();

			void setValue(const float value);

			float getValue() const;

			// set callback for when slider button is pressed
			void setOnButtonPressed(const std::function<void(Voxel::UI::Slider*)>& func);

			// set callback for when slider button is pressed
			void setOnBarPressed(const std::function<void(Voxel::UI::Slider*)>& func);

			// set callback for when slider moves
			void setOnValueChange(const std::function<void(Voxel::UI::Slider*)>& func);

			// set callback for when slider move is finished (mosue release)
			void setOnFinished(const std::function<void(Voxel::UI::Slider*)>& func);
			
			// override
			void updateBoundingBox() override;

			// override
			void updateModelMatrix() override;

			// override
			void updateMouseMoveFalse() override;
			bool updateSliderMouseMove(const glm::vec2& mousePosition, const glm::vec2& mouseDelta);
			bool updateMouseMove(const glm::vec2& mousePosition, const glm::vec2& mouseDelta) override;
			bool updateMousePress(const glm::vec2& mousePosition, const int button) override;
			bool updateMouseRelease(const glm::vec2& mousePosition, const int button) override;

			// render
			void renderSelf() override;

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX && V_DEBUG_DRAW_SLIDER_BOUNDING_BOX
			void createDebugBoundingBoxLine() override;
#endif
		};
	}
}

#endif