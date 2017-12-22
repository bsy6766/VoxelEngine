#ifndef BUTTON_H
#define BUTTON_H

// cpp
#include <array>
#include <functional>

// voxel
#include "UIBase.h"
#include "SpriteSheet.h"

namespace Voxel
{
	namespace UI
	{
		/**
		*	@class Button
		*	@brief A simple button that can be clicked
		*/
		class Button : public RenderNode
		{
		public:
			enum class State
			{
				IDLE = 0,
				HOVERED,
				CLICKED,
				DISABLED
			};
		private:
			Button() = delete;

			// constructor with name
			Button(const std::string& name);

			// Button state. IDLE is default
			State state;

			// frame sizes incase button images are different
			std::array<glm::vec2, 4> frameSizes;

			// gl
			unsigned int currentIndex;

			// On button clicked callback
			std::function<void(Voxel::UI::Button*)> onTriggered;
			// On button cancelled
			std::function<void(Voxel::UI::Button*)> onCancelled;

			/**
			*	Initialize button
			*/
			bool init(SpriteSheet* ss, const std::string& buttonImageFileName);

			/**
			*	Build image.
			*	@param vertices Vertices of image quad.
			*	@param uvs Texture coordinates of image
			*	@param indices Indices of image quad.
			*/
			virtual void build(const std::vector<float>& vertices, const std::vector<float>& uvs, const std::vector<unsigned int>& indices);

			// update mouse move
			bool updateButtonMouseMove(const glm::vec2& mousePosition, const glm::vec2& mouseDelta);
		public:
			// Destructor
			~Button() = default;

			/**
			*	Create button.
			*	@param name Name of button ui
			*	@param spriteSheetName Name of sprite sheet that has button images.
			*	@param buttonImageFileName Image file name of button. Button requires total 4 (idle, hovered, clicked, disabled) images.
			*	buttomImageFileName will be used to load all 4 required iamges by appending '_TYPE' at the end of buttonImageFileName.
			*	For example, buttonImageFileName 'menu_button' or 'menu_button.png' will load 'menu_button_idle.png', 'menu_button_hovered.png',
			*	'menu_button_clicked.png' and 'menu_button_disabled.png'.
			*	All images must be in same sprite sheet.
			*/
			static Button* create(const std::string& name, const std::string& spriteSheetName, const std::string& buttonImageFileName);

			// enable button
			void enable();

			// disable button
			void disable();

			/**
			*	Set callback function for when button is triggered
			*	@param func Callback function to set
			*/
			void setOnTriggeredCallbackFunc(const std::function<void(Voxel::UI::Button*)>& func);

			/**
			*	Set callback function for when button is cancelled
			*	@param func Callback function to set
			*/
			void setOnCancelledCallbackFunc(const std::function<void(Voxel::UI::Button*)>& func);

			// Mouse event overrides
			bool updateMouseMove(const glm::vec2& mousePosition, const glm::vec2& mouseDelta) override;
			bool updateMousePress(const glm::vec2& mousePosition, const int button) override;
			bool updateMouseRelease(const glm::vec2& mousePosition, const int button) override;
			void updateMouseMoveFalse() override;

			// render
			void renderSelf() override;
		};
	}
}

#endif