#ifndef COLOR_PICKER_H
#define COLOR_PICKER_H

// gl
#include <GL\glew.h>

// glm
#include <glm\glm.hpp>

// cpp
#include <string>

// voxel
#include "UIBase.h"

namespace Voxel
{
	// foward declaration
	class Program;
	struct ImageEntry;

	namespace UI
	{
		// foward declaration
		class Canvas;

		class ColorPicker : public RenderNode
		{
		public:
			enum class State
			{
				IDLE = 0,
				HOVERED,
				CLICKED,
			};
		private:
			ColorPicker() = delete;
			ColorPicker(const std::string& name);

			/*
				HSB

				H = [0, 360] in color range

				R		RG		G		GB		B		BR		R
				*-------*-------*-------*-------*-------*-------*
				|                                               |
				*-------*-------*-------*-------*-------*-------*
				0                                               360

				S,B = [0, 100] in pallete range

				<-            S          -> (100, 100)
				*-------------------------* ^
				|                         | |
				|                         |
				|                         |
				|                         |
				|                         |
				|                         | B
				|                         |
				|                         |
				|                         |
				|                         |
				|                         | |
				*-------------------------* v
				(0, 0)

			*/

			// H [0, 360]
			float h;
			// S [0, 1.0f]
			float s;
			// B [0%, 1.0f]
			float b;

			// pallete
			GLuint palleteVao;
			Program* palleteProgram;
			glm::vec2 palleteSize;
			glm::vec4 palleteColor;
			// Uses renderNodes' model mat

			// icon uses Rendernodes' texture, program.
			glm::mat4 iconModelMat;
			glm::vec3 iconColor;

			// state
			State state;

			// set h
			void setH(const float h);
			
			// get hsb(hsv)
			glm::vec3 getHSB() const;

			// get rgb
			glm::vec3 getRGB() const;

			// init
			bool init(const glm::vec2& palleteSize, const std::string& spriteSheetName, const std::string& palleteIconImageName);

			// load pallete buffer
			void loadPalleteBuffer();

			// load icon buffer
			void loadIconBuffer(const Voxel::ImageEntry* ie);

			// update HSB
			void updateHSB(const glm::vec2& mousePosition);

			// update icon pos
			void updateIconPos(const glm::vec2& mousePosition);

			// update color by h
			void updateColor();
			
			// update mouse move
			bool updateColorPickerMouseMove(const glm::vec2& mousePosition, const glm::vec2& mouseDelta);
		public:

			static ColorPicker* create(const std::string& name, const glm::vec2& palleteSize, const std::string& spriteSheetName, const std::string& palleteIconImageName);

			// update mouse move
			bool updateMouseMove(const glm::vec2& mousePosition, const glm::vec2& mouseDelta) override;

			// update mouse press
			bool updateMousePress(const glm::vec2& mousePosition, const int button) override;

			// update mouse release
			bool updateMouseRelease(const glm::vec2& mousePosition, const int button) override;

			// render
			void renderSelf() override;
		};
	}
}

#endif