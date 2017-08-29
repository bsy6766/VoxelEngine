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
	class Program;

	namespace UI
	{
		/**
		*	@class UINode
		*	@brief Base class of any UI classes that contains model matrix(pos, scale in xy and rotation) with pivot data
		*/
		class UINode
		{
		protected:
			UINode();

			// True if visible. Renders object.
			bool visible;

			// Pivot. -0.5f ~ 0.5f. (0, 0) by default
			glm::vec2 pivot;
			// canvas pivot
			glm::vec2 canvasPivot;

			// Position in screen space
			glm::vec2 position;
			// scale in screen sapce
			glm::vec2 scale;
			// rotation
			// glm::vec3 rotation;
			// Model Matrix
			glm::mat4 modelMatrix;

			// Bounding box
			glm::vec2 boxMin;
			glm::vec2 boxMax;

			// Size
			glm::vec2 size;

			virtual void updateMatrix();
		public:
			virtual ~UINode() = default;
			
			virtual void setScale(const glm::vec2& scale);
			virtual void addScale(const glm::vec2& scale);

			virtual void setPosition(const glm::vec2& position);
			virtual void addPosition(const glm::vec2& position);
			virtual glm::vec2 getPosition();

			virtual void setPivot(const glm::vec2& pivot);

			virtual void setVisibility(const bool visibility);
			virtual bool isVisible();

			virtual void setSize(const glm::vec2& size);
			virtual glm::vec2 getSize();

			glm::mat4 getModelMatrix();

			// returns min and max point of box
			glm::vec4 getBoundingBox();

			virtual void setCanvasPivot(const glm::vec2& pivot);
			virtual glm::vec2 getCanvasPivot();
		};
		/**
		*	@class Image
		*	@brief Rectangular png ui image that renders on screen space
		*/
		class Image : public UINode
		{
		private:
			Image();

			Texture2D* texture;

			GLuint vao;
			GLuint vbo;
			GLuint cbo;
			GLuint uvbo;
			GLuint ibo;

			bool init(const std::string& textureName, const glm::vec2& screenPosition, const glm::vec4& color);
		public:
			~Image();

			static Image* create(const std::string& textureName, const glm::vec2& screenPosition, const glm::vec4& color);

			void render(const glm::mat4& screenMat, const glm::mat4& canvasPivotMat, Program* prog);
		};

		/**
		*	@class Text
		*	@brief List of quads where each quad renders each character.
		*
		*	Text class builds list of quads one by one, based on font metric.
		*	It doesn't provide any font related function, such as linespace or size modification.
		*	Use FontManager to get font and modify values from there.
		*/
		class Text : public UINode
		{
		public:
			enum class ALIGN
			{
				LEFT = 0,
				CENTER,
				RIGHT
			};

			enum class TYPE
			{
				STATIC = 0,
				DYNAMIC
			};
		private:
			Text();

			bool loaded;

			std::string text;

			Font* font;

			GLuint vao;
			GLuint vbo;
			GLuint cbo;
			GLuint uvbo;
			GLuint ibo;

			TYPE type;

			unsigned int indicesSize;
			int maxTextLength;
			
			float maxWidth;
			float totalHeight;
			
			ALIGN align;

			bool outlined;

			bool init(const std::string& text, const glm::vec2& position, const int fontID, ALIGN align = ALIGN::LEFT, TYPE type = TYPE::STATIC, const int maxLength = 0);
			bool buildMesh(const bool update);
			void loadBuffers(const std::vector<float>& vertices, const std::vector<float>& colors, const std::vector<float>& uvs, const std::vector<unsigned int>& indices);
			void updateBuffer(const std::vector<float>& vertices, const std::vector<float>& colors, const std::vector<float>& uvs, const std::vector<unsigned int>& indices);
			std::vector<glm::vec2> computeOrigins(Font* font, const std::vector<std::string>& split);
		public:
			~Text();

			static Text* create(const std::string& text, const glm::vec2& position, const int fontID, ALIGN align = ALIGN::LEFT, TYPE type = TYPE::STATIC, const int maxLength = 0);

			void setText(const std::string& text);

			bool isOutlined();

			void render(const glm::mat4& screenMat, const glm::mat4& canvasPivotMat, Program* prog);
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
			Canvas();

			bool visible;

			static const float fixedFovy;

			glm::vec2 size;
			glm::vec2 centerPosition;

			// Each ui components
			std::unordered_map<std::string, Image*> images;
			std::unordered_map<std::string, Text*> texts;
		public:
			~Canvas();

			static Canvas* create(const glm::vec2& size, const glm::vec2& centerPosition);

			// add image
			bool addImage(const std::string& name, const std::string& textureName, const glm::vec2& position, const glm::vec4& color);
			bool addImage(const std::string& name, Image* image, const int z);

			// add test
			bool addText(const std::string& name, const std::string& text, const glm::vec2& position, const int fontID, Text::ALIGN align = Text::ALIGN::LEFT, Text::TYPE type = Text::TYPE::STATIC, const int maxLength = 0);
			bool addText(const std::string& name, Text* text, const int z);

			// Render all UI objects
			void render();

			Image* getImage(const std::string& name);
			Text* getText(const std::string& name);

			glm::vec2 getPivotCanvasPos(PIVOT pivot);

			void setSize(const glm::vec2& size);

			void setVisibility(const bool visibility);
		};

	}
}

#endif