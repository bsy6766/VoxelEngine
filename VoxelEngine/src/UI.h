#ifndef UI_H
#define UI_H

#include <GL\glew.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <map>
#include <ZOrder.h>
#include <memory>
#include <Geometry.h>

namespace Voxel
{
	class Texture2D;
	class Font;
	class Program;
	class SpriteSheet;

	namespace UI
	{
		/**
		*	@class UINode
		*	@brief Base class of any UI classes that contains model matrix(pos, scale in xy and rotation) with pivot data
		*/
		class UINode
		{
		protected:
			// True if visible. Renders object.
			bool visible;

			// Pivot. -0.5f ~ 0.5f. (0, 0) by default
			glm::vec2 pivot;

			// string name
			std::string name;

			// canvas pivot
			//glm::vec2 canvasPivot;

			// Position in screen space
			glm::vec2 position;

			// scale in screen sapce
			glm::vec2 scale;

			// Model Matrix
			glm::mat4 modelMatrix;

			// Size
			glm::vec2 size;

			// Opacity
			float opacity;

			// childrens
			std::map<ZOrder, std::unique_ptr<UINode>, ZOrderComp> children;

			// Find next ZOrder
			bool getNextZOrder(ZOrder& curZOrder);

			virtual void updateMatrix();
		public:
			UINode(const std::string& name);
			virtual ~UINode() = default;

			// add child
			bool addChild(UINode* child);
			bool addChild(UINode* child, int zOrder);
			bool addChild(UINode* child, ZOrder& zOrder);
			
			// Scale
			virtual void setScale(const glm::vec2& scale);
			virtual void addScale(const glm::vec2& scale);

			// Position
			virtual void setPosition(const glm::vec2& position);
			virtual void addPosition(const glm::vec2& position);
			virtual glm::vec2 getPosition();

			// Pivot
			virtual void setPivot(const glm::vec2& pivot);

			// Visibility
			virtual void setVisibility(const bool visibility);
			virtual bool isVisible();

			// Size
			virtual void setSize(const glm::vec2& size);
			virtual glm::vec2 getSize();

			// Opacity (0 ~ 1.0f)
			void setOpacity(const float opacity);
			float getOpacity();
			 
			// Name. Can't be empty string
			void setName(const std::string& name);
			std::string getName();

			// Model Matrix
			glm::mat4 getModelMatrix();

			// returns min and max point of box
			Geometry::Rect getBoundingBox();

			// Check if this node has children
			bool hasChildren();

			//virtual void setCanvasPivot(const glm::vec2& pivot);
			//virtual glm::vec2 getCanvasPivot();

			// Print children
			void printChildren(const int depth);
		};

		/**
		*	@class UIBatch
		*	@brief Batches the UI objets that shares shader and texture.
		*
		*	All UI component except Text is batched. 
		*	Text uses font texture atlas and have high chance to be modified. 
		*/
		class UIBatch
		{
			friend class Canvas;
		private:
			// Constructor
			UIBatch();
			// Destructor. Releases batch. Calls OpenGL calls.
			~UIBatch();

			// Vertex array object
			GLuint vao;

			// size if indices
			unsigned int indicesSize;

			// Texture
			Texture2D* texture;

			// Program
			Program* program;

			// Render batch.
			void render();
		};

		/**
		*	@class Image
		*	@brief Rectangular png ui image that renders on screen space
		*
		*	Simple image that renders in screen space.
		*	Image will be static and won't be changed nor animated
		*/
		class Image : public UINode
		{
		private:
			Image();

			Texture2D* texture;

			GLuint vao;

			bool init(const std::string& textureName, const glm::vec2& screenPosition, const glm::vec4& color);
			bool initFromSpriteSheet(SpriteSheet* ss, const std::string& textureName, const glm::vec2& screenPosition, const glm::vec4& color);
		public:
			~Image();

			static Image* create(const std::string& textureName, const glm::vec2& screenPosition, const glm::vec4& color);
			static Image* createFromSpriteSheet(const std::string& spriteSheetName, const std::string& textureName, const glm::vec2& screenPosition, const glm::vec4& color);

			void render(const glm::mat4& screenMat, const glm::mat4& canvasPivotMat, Program* prog);
		};

		/**
		*	@class Canvas
		*	@brief A rectangular shape of area that defines screen space for UI.
		*
		*	Canvas is a rectangular area where UI component can be placed and rendered.
		*	UI objects are sorted based on ZOrder. If you don't provide specific ZOrder, 
		*	it will assign new one starting from 0. If ZOrder is maxed, then it rejects
		*	new UI objects.
		*
		*	Each UI objects requires to have texture in single texture file for performance.
		*	If multiple UI objects shares same texture, it will be batched in single
		*	draw call. 
		*
		*	Batching in UI isn't perfect. It's based on how many objects that shares
		*	shader and texture are continued. However, most of the time shader and
		*	texture are shared. 
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

			glm::vec2 size;
			glm::vec2 position;

			// Find next ZOrder
			bool getNextZOrder(ZOrder& curZOrder);

			// UI objects
			std::map<ZOrder, std::unique_ptr<UINode>, ZOrderComp> uiNodes;
		public:
			// Destructor. Releases all UIs.
			~Canvas();

			/**
			*	Create canvas. 
			*	@param [in] size Size of canvas. Must be positive.
			*	@param [in] position The position of canvas.
			*	@return Canvas instance. Nullptr if fails.
			*/
			static Canvas* create(const glm::vec2& size, const glm::vec2& position);

			// Add node
			bool addNode(UINode* node);
			bool addNode(UINode* node, int zOrder);
			bool addNode(UINode* node, ZOrder zOrder);
			/*
			// add image
			//bool addImage(const std::string& name, const std::string& textureName, const glm::vec2& position, const glm::vec4& color);
			bool addImage(const std::string& name, Image* image, const int z);

			// add test
			//bool addText(const std::string& name, const std::string& text, const glm::vec2& position, const glm::vec4& color, const int fontID, Text::ALIGN align = Text::ALIGN::LEFT, Text::TYPE type = Text::TYPE::STATIC, const int maxLength = 0);
			bool addText(const std::string& name, Text* text, const int z);


			Image* getImage(const std::string& name);
			Text* getText(const std::string& name);

			glm::vec2 getPivotCanvasPos(PIVOT pivot);
			*/

			/**
			*	get the size of canvas
			*	@return Size of canvas in vec2.
			*/
			glm::vec2 getSize();

			/**
			*	Set the size of canvas. 
			*	@param [in] size A size of canvas. Rejects if size is negative.
			*/
			void setSize(const glm::vec2& size);

			/**
			*	Get visibility of canvas
			*	return True if canvas is visible
			*/
			bool getVisibility();

			/**
			*	Set visibility of canvas. Disabling visibility will hide all UI components in this canvas.
			*/
			void setVisibility(const bool visibility);

			/**
			*	Updates mouse input. Checks if mouse is hovering UI or clicked UI
			*	@param [in] mousePos Position of mouse cursor.
			*	@param [in] mouseButton A GLFW mouse button ID.
			*/
			void updateMouseInput(const glm::vec2& mousePos, int mouseButton);

			/**
			*	Renders UI objects.
			*/
			void render();

			/**
			*	Print canvas info.
			*/
			void print();
		};
	}
}

#endif