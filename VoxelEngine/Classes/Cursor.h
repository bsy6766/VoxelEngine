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
	/**
	*	@class Cursor
	*	@brief Textured cursor. Can switch to different cursors. Renders in screen space
	*	
	*	Singleton because cursor always exists from start to end of app.
	*	Creation and deletion managed by Application class.
	*	Cursor's position gets updated by Application class.
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
		// Delete copy, move, assign operators
		Cursor(Cursor const&) = delete;             // Copy construct
		Cursor(Cursor&&) = delete;                  // Move construct
		Cursor& operator=(Cursor const&) = delete;  // Copy assign
		Cursor& operator=(Cursor &&) = delete;      // Move assign
	public:
		static Cursor& getInstance()
		{
			static Cursor instance;
			return instance;
		}
	private:
		// Constructor
		Cursor();
		// Destructor
		~Cursor();

		// Textures for cursor.
		Texture2D* texture;

		// position of cursor
		glm::vec2 position;

		// Pivot. -0.5f ~ 0.5f.
		glm::vec2 pivot;

		// size is fixed to 32 x 32
		const glm::vec2 size = glm::vec2(32.0f, 32.0f);

		// lock. If cursor is locked, user only can move cursor in the window. If cursor unlocked, 

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

		// check ursor boundary
		void checkBoundary();
	public:
		// Initailize all cursors
		bool init();

		// release cursors. This doesn't deletes instance.
		void release();

		/**
		*	Add position to current cursor position
		*	@param distance Amount of distance to add to cursor's position
		*/
		void addPosition(const glm::vec2& distance);

		/**
		*	Set position
		*	@param position New cursor position
		*/
		void setPosition(const glm::vec2& position);

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

		// check if cursor is visible
		bool isVisible() const;

		/**
		*	Get current position of cursor in screen space
		*/
		glm::vec2 getPosition() const;

		/**
		*	Renders cursor
		*/
		void render();
	};
}

#endif