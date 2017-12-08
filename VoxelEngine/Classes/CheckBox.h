#ifndef CHECK_BOX_H
#define CHECK_BOX_H

// voxel
#include "UIBase.h"
#include "SpriteSheet.h"

namespace Voxel
{
	namespace UI
	{
		/**
		*	@class CheckBox
		*	@brief A simple checkbox that can be checked or unchekced
		*/
		class CheckBox : public RenderNode
		{
		public:
			enum class State
			{
				DESELECTED = 0,
				HOVERED,
				CLICKED,
				SELECTED,
				HOVERED_SELECTED,
				CLICKED_SELECTED,
				DISABLED
			};
		private:
			// Constructor
			CheckBox() = delete;
			CheckBox(const std::string& name);

			// state
			State prevCheckBoxState;
			State checkBoxState;

			// gl
			unsigned int currentIndex;

			/**
			*	Initialize button
			*/
			bool init(SpriteSheet* ss, const std::string& checkBoxImageFileName);

			/**
			*	Build image.
			*	Initialize vao.
			*	@param vertices Vertices of image quad
			*	@param uvs Texture coordinates of image quad
			*	@param indices Indices of image quad
			*/
			void build(const std::vector<float>& vertices, const std::vector<float>& uvs, const std::vector<unsigned int>& indices);

			/**
			*	Update current index based on state
			*/
			void updateCurrentIndex();

			// update mouse move
			bool updateMouseMove(const glm::vec2& mousePosition);
		public:
			// Destructor
			~CheckBox() = default;

			/**
			*	Create checkbox
			*	@param name Name of ui
			*	@param spriteSheetName Name of sprite sheet that has check box ui images
			*	@param checkBoxImageFileName Image file name of check box. This will used to load check box ui images
			*/
			static CheckBox* create(const std::string& name, const std::string& spriteSheetName, const std::string& checkBoxImageFileName);

			/**
			*	Enable button.
			*/
			void enable();

			/**
			*	Disable button
			*/
			void disable();

			/**
			*	Select check box manually. Ignored when check box is disabled.
			*/
			void select();

			/**
			*	Deselect check box manually. Ignored when check box is disabled.
			*/
			void deselect();

			/**
			*	Check if mouse is hovering button
			*/
			bool updateMouseMove(const glm::vec2& mousePosition, const glm::vec2& mouseDelta) override;

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