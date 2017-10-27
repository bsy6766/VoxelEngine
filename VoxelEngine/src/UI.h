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
	struct ImageEntry;

	namespace UI
	{
		class Canvas;

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

			// parent pivot. 
			glm::vec2 parentPivot;

			// Parent
			Canvas* parentCanvas;

			// Position in screen space
			glm::vec2 position;

			// scale in screen sapce
			glm::vec2 scale;

			// Model Matrix
			glm::mat4 modelMatrix;

			// Size
			glm::vec2 size;

			// color
			glm::vec3 color;

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

			// get Texture
			virtual Texture2D* getTexture();

			// Build vertices
			virtual void buildVertices(std::vector<float>& bVertices, std::vector<unsigned int>& bIndices, std::vector<float>& bColors, std::vector<float>& bUVs, const unsigned int indexStart, const glm::mat4& parentMat) = 0;

			// Color
			void setColor(const glm::vec3& color);
			glm::vec3 getColor();

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

			// parent pivot
			virtual void setParentPivot(const glm::vec2& pivot);
			virtual glm::vec2 getParentPivot();

			// Check if this node has children
			bool hasChildren();

			// Checks if this object can be rendered
			virtual bool isRenderable();

			virtual unsigned int getIndicesOffset() = 0;

			// Get children in vector. negative z order -> parent -> positive z order
			void getChildrenInVector(std::vector<UINode*>& nodes, UINode* parent);

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

			// load
			void load(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, const std::vector<float>& colors, const std::vector<float>& uvs);

			// Render batch.
			void render();
		};

		/**
		*	@class Cursor
		*	@brief Textured cursor. Can switch to different cursors. Renders in screen space
		*
		*	Unlike other UIs, cursor doesn't have parent canvas. Cursor is universal nomatter what the game state is at, so Game class manages the cursor.
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

			GLuint vao;
			GLuint uvbo;

			// visibility
			bool visible;

			// screen boundary
			glm::vec2 minScreenBoundary;
			glm::vec2 maxScreenBoundary;

			// Current cursor type
			CursorType currentCursorType;

			float screenSpaceZ;

			// Initailize all cursors
			bool init();
		public:
			~Cursor();

			// Creates cursor.
			static Cursor* create();

			// position
			void setPosition(const glm::vec2& position);
			void addPosition(const glm::vec2& distance);

			// set boundary
			void updateBoudnary();

			// Set cursor type. 
			void setCursorType(const CursorType cursorType);

			// Set visibility of all cursor
			void setVisibility(const bool visibility);

			// Render cursor
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
			// Constructor with name
			Image(const std::string& name);

			// Texture
			Texture2D* texture;

			// Cache uv because it varys based on texture.
			std::vector<float> uvs;

			// Initialize image
			bool init(const std::string& textureName, const glm::vec2& position, const glm::vec3& color, const float opacity);
			bool initWithSpriteSheet(const ImageEntry* imageEntry, Texture2D* texture, const glm::vec2& position, const glm::vec3& color = glm::vec3(1.0f), const float opacity = 1.0f);

		public:
			// Default constructor
			~Image() = default;

			/**
			*	Creates Image. Uses existing texture if exists. Else, creates new.
			*	@param [in] name Name of object. Can't be empty.
			*	@param [in] textureName Name of texture. Can't be empty.
			*	@param [in] position Position of Image in canvas.
			*	@param [in] color Color (rgb) of image. White by default.
			*	@param [in] opacity Opacity of image. 1.0f by default.
			*	@return An Image instance. Nullptr if fails.
			*/
			static Image* create(const std::string& name, const std::string& textureName, const glm::vec2& position, const glm::vec3& color = glm::vec3(1.0f), const float opacity = 1.0f);
			
			/**
			*	Creates Image from sprite sheet
			*	@param [in] name Name of object. Can't be empty.
			*	@param [in] spriteSheetName Name of sprite sheet. can't be empty.
			*	@param [in] imageName Name of image. can't be empty.
			*	@param [in] position Position of Image in canvas.
			*	@param [in] color Color (rgb) of image. White by default.
			*	@param [in] opacity Opacity of image. 1.0f by default.
			*	@return An Image instance. Nullptr if fails.
			*/
			static Image* createWithSpriteSheet(const std::string& name, const std::string& spriteSheetName, const std::string& imageName, const glm::vec2& position, const glm::vec3& color = glm::vec3(1.0f), const float opacity = 1.0f);

			/**
			*	Build vertices for image (quad)
			*	@param [in] bVertices Vertices that are batched
			*	@param [in] bIndices Indices that are batched
			*	@param [in] bColors Colors that are batched
			*	@param [in] bUV UVs Coordinates that are batched
			*/
			void buildVertices(std::vector<float>& bVertices, std::vector<unsigned int>& bIndices, std::vector<float>& bColors, std::vector<float>& bUVs, const unsigned int indexStart, const glm::mat4& parentMat) override;

			// Check if this image is renderable, which is always true though.
			bool isRenderable() override;

			unsigned int getIndicesOffset() override;

			// get texture
			Texture2D* getTexture() override;
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

			// True if there was modification on uiNodes
			bool needToUpdateBatch;

			glm::vec2 size;
			glm::vec2 position;

			// Find next ZOrder
			bool getNextZOrder(ZOrder& curZOrder);

			// UI objects
			std::map<ZOrder, std::unique_ptr<UINode>, ZOrderComp> uiNodes;

			// Batches
			std::vector<UIBatch*> batches;
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

			//glm::vec2 getPivotCanvasPos(PIVOT pivot);

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
			*	Updates batch only if it's needed
			*/
			void updateBatch();

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