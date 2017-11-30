#ifndef TEXT_H
#define TEXT_H

#include "UIBase.h"

namespace Voxel
{
	// foward declaration
	class Font;

	namespace UI
	{
		/**
		*	@class Text
		*	@brief List of quads where each quad renders each character.
		*
		*	Text class builds list of quads one by one, based on font metric.
		*	It doesn't provide any font related function, such as linespace or size modification.
		*	Use FontManager to get font and modify values from there.
		*/
		class Text : public RenderNode
		{
		public:
			// Text align
			enum class ALIGN
			{
				LEFT = 0,
				CENTER,
				RIGHT
			};
		private:
			Text() = delete;

			// Constructor
			Text(const std::string& name);

			// String text
			std::string text;

			// Pointer to font
			Font* font;

			// gl
			GLuint vbo;
			GLuint cbo;
			GLuint uvbo;
			GLuint ibo;
			unsigned int indicesSize;

			// Color of text
			glm::vec3 color;

			// Color of outline
			glm::vec3 outlineColor;

			// align
			ALIGN align;

			// outline
			bool outlined;

			/**
			*	Initialize text.
			*/
			bool init(const std::string& text, const ALIGN align = ALIGN::LEFT);

			/**
			*	Initialize text with outline
			*/
			bool initWithOutline(const std::string& text, const glm::vec3& outlineColor, ALIGN align = ALIGN::LEFT);

			/**
			*	Builds mesh and loads.
			*	@param update true if it's building mesh for updating text. false if it's newly
			*/
			bool buildMesh(const bool update);

			/**
			*	Load buffers.
			*	@param vertices Text vertices.
			*	@param color Text colors.
			*	@param uvs Texture coordinates for each character
			*	@param indices Indices for rendering
			*	@param update true if it needs to reallocate buffer. Else, false.
			*/
			void loadBuffers(const std::vector<float>& vertices, const std::vector<float>& colors, const std::vector<float>& uvs, const std::vector<unsigned int>& indices, const bool update);

			/**
			*	Computes the origin point for each line
			*	Origin point equals to point where guide lines start in real life notebooks.
			*/
			std::vector<glm::vec2> computeOrigins(Font* font, const std::vector<std::string>& split);

		public:
			// Destructor
			~Text();

			/**
			*	Create text
			*	@param name Name of ui
			*	@param text A string text to render
			*	@param fontID Font id to use
			*	@param align Text align. Left by default.
			*	@return Text ui if successfully loads text to render. Else, nullptr
			*/
			static Text* create(const std::string& name, const std::string& text, const int fontID, const ALIGN align = ALIGN::LEFT);

			/**
			*	Create text
			*	@param name Name of ui
			*	@param text A string text to render
			*	@param fontID Font id to use
			*	@param outlineColor Color of text outline
			*	@param align Text align. Left by default.
			*	@return Text ui if successfully loads text to render. Else, nullptr
			*/
			static Text* createWithOutline(const std::string& name, const std::string& text, const int fontID, const glm::vec3& outlineColor = glm::vec3(0.0f), const ALIGN align = ALIGN::LEFT);

			/**
			*	Sets text.
			*	Rebuild buffer and updates
			*	@param text A string text to set.
			*/
			void setText(const std::string& text);

			/**
			*	Get text
			*/
			std::string getText() const;

			/**
			*	Checks if text is outlined
			*	@return true if text is outlined. Else, false.
			*/
			bool isOutlined() const;

			/**
			*	Set text color. White by default
			*	@param color Color of text in range of 0.0 ~ 1.0
			*/
			void setColor(const glm::vec4& color);

			/**
			*	Set outline color. Black by default
			*	@param color Color of text in range of 0.0 ~ 1.0
			*/
			void setOutlineColor(const glm::vec4& color);

			/**
			*	Get outline color
			*/
			glm::vec3 getOutlineColor() const;

			/**
			*	Clear text.
			*/
			void clear();

			/**
			*	Render self
			*/
			void renderSelf() override;
		};
	}
}

#endif