#ifndef CURSOR_H
#define CURSOR_H

// voxel
#include "Texture2D.h"

// gl
#include <GL\glew.h>

// glm
#include <glm\glm.hpp>

namespace Voxel
{
	namespace UI
	{

		/**
		*	@class Cursor
		*	@brief Textured cursor. Can switch to different cursors. Renders in screen space
		*
		*	Unlike other UIs, cursor doesn't have parent canvas. It's universal for all canvases. Therefore cursor is managed by Game class.
		*	Todo: Instead of updating uv buffer, intialize all cursor quads and just render part of vao.
		*/
		class Cursor
		{
		public:
			enum class CursorType
			{
				POINTER = 0,	// Default cursor
				FINGER,
			};
		private:
			Cursor();

			// Textures for cursor.
			Texture2D* texture;

			// position of cursor
			glm::vec2 position;

			// Pivot. -0.5f ~ 0.5f.
			glm::vec2 pivot;

			// size is fixed to 32 x 32
			const glm::vec2 size = glm::vec2(32.0f, 32.0f);

			// gl
			GLuint vao;
			GLuint uvbo;

			// visibility
			bool visible;

			// screen boundary
			glm::vec2 minScreenBoundary;
			glm::vec2 maxScreenBoundary;

			// Current cursor type
			CursorType currentCursorType;

			// Initailize all cursors
			bool init();
		public:
			// Destructor
			~Cursor();

			// Creates cursor.
			static Cursor* create();

			/**
			*	Add position to current cursor position
			*	@param distance Amount of distance to add to cursor's position
			*/
			void addPosition(const glm::vec2& distance);

			/**
			*	Updates boundary. Call this whenever window size changes.
			*/
			void updateBoundary();

			/**
			*	Set cursor type
			*	@param cursorType Type of cursor.
			*/
			void setCursorType(const CursorType cursorType);

			/**
			*	Toggle visibilty of cursor
			*	@param visibility true to show cursor. Else, false.
			*/
			void setVisibility(const bool visibility);

			/**
			*	Get current position of cursor in screen space
			*/
			glm::vec2 getPosition() const;

			/**
			*
			*/
			void render();
		};
	}
}

#endif