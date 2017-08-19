#ifndef UI_H
#define UI_H

#include <GL\glew.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

namespace Voxel
{
	class Texture2D;
	class Font;

	namespace UI
	{
		/**
		*	@class Image
		*	@brief Rectangular png ui image that renders on screen space
		*/
		class Image
		{
		private:
			Image();

			bool visible;
			Texture2D* texture;

			glm::vec2 position;

			GLuint vao;
			GLuint vbo;
			GLuint cbo;
			GLuint uvbo;
			GLuint ibo;

			bool init(const std::string& textureName, const glm::vec2& screenPosition);
		public:
			~Image();
			static Image* create(const std::string& textureName, const glm::vec2& screenPosition);

			void update(const float delta);
			void render();
		};

		/**
		*	@class Text
		*	@brief List of quads where each quad renders each character.
		*
		*	Text class builds list of quads one by one, based on font metric.
		*	It doesn't provide any font related function, such as linespace or size modification.
		*	Use FontManager to get font and modify values from there.
		*/
		class Text
		{
		public:
			enum class ALIGN
			{
				LEFT = 0,
				CENTER,
				RIGHT
			};
		private:
			Text();

			bool visible;

			glm::vec2 position;

			std::string text;

			Font* font;

			GLuint vao;
			GLuint vbo;
			GLuint cbo;
			GLuint uvbo;
			GLuint ibo;

			unsigned int indicesSize;
			
			float maxWidth;
			float totalHeight;
			
			ALIGN align;

			bool init(const std::string& text, const glm::vec2& position, const int fontID, ALIGN align = ALIGN::LEFT);
			bool buildMesh(const int fontID);
			std::vector<glm::vec2> computeOrigins(Font* font, const std::vector<std::string>& split);
		public:
			~Text();

			static Text* create(const std::string& text, const glm::vec2& position, const int fontID, ALIGN align = ALIGN::LEFT);

			void render();
		};

		/**
		*	@class Canvas
		*	@brief A rectangular shape of area that defines screen space for UI.
		*
		*	Canvas can have different UI components such as image or text and renders
		*	each type of component in single draw call if they share the same texture.
		*	So it's good to put all the UI textures in single texture.
		*/
		class Canvas
		{
		private:
			Canvas();

			glm::vec2 screenSize;
			glm::vec2 centerPosition;

			// Each ui components
			std::unordered_map<std::string, Image*> images;
			std::unordered_map<std::string, Text*> texts;
		public:
			~Canvas();

			static Canvas* create(const glm::vec2& screenSize, const glm::vec2& centerPosition);

			// add image
			bool addImage(const std::string& name, const std::string& textureName, const glm::vec2& position);
			bool addImage(const std::string& name, Image* image, const int z);

			// add test
			bool addText(const std::string& name, const std::string& text, const glm::vec2& position, const int fontID, Text::ALIGN align = Text::ALIGN::LEFT);
			bool addText(const std::string& name, Text* text, const int z);

			// Render all UI objects
			void render();
		};

	}
}

#endif