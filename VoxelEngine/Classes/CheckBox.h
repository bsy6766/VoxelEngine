#ifndef CHECK_BOX_H
#define CHECK_BOX_H

// cpp
#include <functional>

// voxel
#include "RenderNode.h"
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
			State prevState;
			State state;

			// gl
			unsigned int currentIndex;

			// On checkbox selected callback
			std::function<void(Voxel::UI::CheckBox* sender)> onSelected;
			
			// On checkbox deselected callback
			std::function<void(Voxel::UI::CheckBox* sender)> onDeselected;

			// On checkbox cancelled callback
			std::function<void(Voxel::UI::CheckBox* sender)> onCancelled;

			// initialize
			bool init(SpriteSheet* ss, const std::string& checkBoxImageFileName);

			// Build gl.
			void build(const std::vector<float>& vertices, const std::vector<float>& uvs, const std::vector<unsigned int>& indices);

			// Update current vertex index based on state
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

			// Enable checkbox
			void enable();

			// Disable checkbox
			void disable();

			// select check box
			void select();

			// Deselect check box
			void deselect();

			/**
			*	Set callback function for when checkbox is selected
			*	@param func Callback function to set
			*/
			void setOnSelectedCallbackFunc(const std::function<void(Voxel::UI::CheckBox*)>& func);

			/**
			*	Set callback function for when button is deselected
			*	@param func Callback function to set
			*/
			void setOnDeselectedCallbackFunc(const std::function<void(Voxel::UI::CheckBox*)>& func);

			/**
			*	Set callback function for when button is cancelled
			*	@param func Callback function to set
			*/
			void setOnCancelledCallbackFunc(const std::function<void(Voxel::UI::CheckBox*)>& func);

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