#ifndef UI_H
#define UI_H

#include <GL\glew.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

namespace Voxel
{
	class Texture2D;

	namespace UI
	{
		/**
		*	@class Image
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

			// Store all the images in this canvas.
			std::unordered_map<std::string, Image*> images;
		public:
			~Canvas();

			static Canvas* create(const glm::vec2& screenSize, const glm::vec2& centerPosition);

			void addImage(const std::string& name, const std::string& textureName, const glm::vec2& position);
			void addImage(const std::string& name, Image* image, const int z);

			// Render all UI objects
			void render();
		};

	}
}

#endif