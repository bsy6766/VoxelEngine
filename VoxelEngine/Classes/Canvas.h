#ifndef CANVAS_H
#define CANVAS_H

#include "UIBase.h"

namespace Voxel
{
	namespace UI
	{
		/**
		*	@class Canvas
		*	@brief A rectangular shape of area that defines screen space for UI.
		*/
		class Canvas : public TransformNode
		{
		public:
			enum class PIVOT
			{
				CENTER = 0,
				LEFT,
				RIGHT,
				TOP,
				BOTTOM,
				LEFT_TOP,
				LEFT_BOTTOM,
				RIGHT_TOP,
				RIGHT_BOTTOM
			};
		private:
			// Deletes default constructor
			Canvas() = delete;

			// Visibility. true if visible. false if invisible and ignores opaicty
			bool visibility;

			// Size of ui space
			glm::vec2 size;

			// Center position of canvas in screen space. Center of monitor screen is 0
			glm::vec2 centerPosition;

			// Override
			void updateModelMatrix() override;

			// override
			glm::mat4 getModelMatrix() override;
		public:
			// Constructor
			Canvas(const glm::vec2& size, const glm::vec2& centerPosition);

			// Destructor.
			~Canvas() = default;

			/**
			*	Set visibility
			*	Setting visibility false will also affect all the children
			*	@parma visibility true to render this ui. false to not render.
			*/
			void setVisibility(const bool visibility);

			/**
			*1	Get visibility
			*/
			bool getVisibility() const;

			/**
			*	Set size of canvas
			*	@param size Size of canvas. Must be positive numbers
			*/
			void setSize(const glm::vec2& size);

			/**
			*	Update all UI
			*/
			void update(const float delta);

			/**
			*	Update mouse movement.
			*	@param mosuePosition Current position of mouse in screen space
			*/
			bool updateMouseMove(const glm::vec2& mousePosition, const glm::vec2& mouseDelta) override;

			/**
			*	update mouse click
			*	@param mousePosition Current position of mouse in screen space
			*	@param button Clciked mouse button. 0 = left, 1 = right, 2 = middle
			*	@return true if mouse clicked on ui. Else, false.
			*/
			bool updateMousePress(const glm::vec2& mousePosition, const int button) override;

			/**
			*	update mouse released
			*	@param mousePosition Current position of mouse in screen space
			*	@param button Release mouse button. 0 = left, 1 = right, 2 = middle
			*	@return true if mouse released on ui. Else, false.
			*/
			bool updateMouseRelease(const glm::vec2& mousePosition, const int button) override;

			/**
			*	Render all UI
			*/
			void render() override;

			// print
			void print(const int tab) override;
		};
	}
}

#endif