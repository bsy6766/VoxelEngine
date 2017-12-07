#ifndef BUTTON_H
#define BUTTON_H

#include "UIBase.h"

// cpp
#include <array>

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
			State buttonState;

			// frame sizes incase button images are different
			std::array<glm::vec2, 4> frameSizes;

			// gl
			unsigned int currentIndex;

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

			/**
			*	Enable button.
			*/
			void enable();

			/**
			*	Disable button
			*/
			void disable();

			/**
			*	Check if mouse is hovering button
			*/
			void updateMouseMove(const glm::vec2& mousePosition) override;

			/**
			*	Check if mouse clicked the button
			*/
			bool updateMouseClick(const glm::vec2& mousePosition, const int button) override;

			/**
			*	Check if mouse released the button
			*/
			bool updateMouseRelease(const glm::vec2& mousePosition, const int button) override;

			/**
			*	Render self
			*/
			void renderSelf() override;
		};
	}
}

#endif