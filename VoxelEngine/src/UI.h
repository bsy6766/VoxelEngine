#ifndef UI_H
#define UI_H

// Config
#include <Config.h>

// glm
#include <glm/glm.hpp>

// gl
#include <GL\glew.h>

// cpp
#include <string>
#include <unordered_map>
#include <map>
#include <memory>

// Voxel
#include <ZOrder.h>
#include <Shape.h>

namespace Voxel
{
	// Forward declarations
	class Texture2D;
	class Font;
	class Program;
	class SpriteSheet;

	namespace UI
	{
		// Forward declaration
		class Node;
		class Image;
		class AnimatedImage;
		class Text;
		class Button;
		class ProgressTimer;
		class Sequence;
		class Action;
		class ActionPack;
		class MoveTo;
		class RotateTo;
		class ScaleTo;
		class FadeTo;
		class Delay;
		class ProgressTo;

		typedef std::map<ZOrder, std::unique_ptr<Node>, ZOrderComp> Children;

		/**
		*	@class Node
		*	A base class to all UI component, including Canvas.
		*
		*	Contains transformation data (position, rotation, scale, opacity) and can run actions.
		*	Bounding boxes are auto generated based on the size of content.
		*/
		class Node
		{
		private:
			// Find next ZOrder
			bool getNextZOrder(ZOrder& curZOrder);
		protected:
			// Constructor
			Node();
			Node(const std::string& name);

			// Name of ui
			std::string name;

			// Z order of ui
			ZOrder zOrder;

			// Visibility. true if visible. false if invisible and ignores opaicty
			bool visibility;

			// Opacity
			float opacity;

			// Position
			glm::vec2 position;

			// Rotation
			float angle;

			// Scale
			glm::vec2 scale;

			// Pivot
			glm::vec2 pivot;
			
			// Model matrix
			glm::mat4 modelMat;

			// Bounding box
			Voxel::Shape::Rect boundingBox;

			// Action sequence
			Voxel::UI::Sequence* sequence;

			// Children
			Children children;

			// Program ptr
			Program* program;

			// Updates model matrix
			void updateModelMatrix();

		public:
			virtual ~Node();

			/**
			*	Set opacity
			*	@param opacity Value of new opacity. Must be 0.0 ~ 1.0
			*/
			void setOpacity(const float opacity);

			/**
			*	Get opacity. 
			*/
			float getOpacity() const;

			/**
			*	Set position
			*	@param position New position to set. 
			*/
			void setPosition(const glm::vec2& position);

			/**
			*	Get position of ui
			*/
			glm::vec2 getPosition() const;

			/**
			*	Set angle of ui
			*	@param angle Angle of ui in degree. Angle gets clamped to 0.0 to 359.99
			*/
			void setAngle(const float angle);

			/**
			*	Get angle of ui
			*/
			float getAngle() const;

			/**
			*	Set scale of ui.
			*	@param scale Scale of ui in x and y axis. Default value is 1.0. Must be positive number.
			*/
			void setScale(const glm::vec2& scale);

			/**
			*	Get scale of ui.
			*/
			glm::vec2 getScale() const;

			/**
			*	Set pivot of ui
			*	@param pivot Pivot of ui to set. Default value is 0.0
			*/
			void setPivot(const glm::vec2& pivot);

			/**
			*	Get pivot of ui
			*/
			glm::vec2 getPivot() const;

			/**
			*	Get bounding box. 
			*/

			/**
			*	Set Z order
			*	@param zOrder A new z order to set.
			*/
			void setZorder(const ZOrder& zOrder);

			/**
			*	Get Z order
			*/
			ZOrder getZOrder() const;

			/**
			*	Add child to this ui node
			*	@param child Child node to add.
			*/
			bool addChild(Node* child);

			/**
			*	Add child with global z order
			*	@param child Child node to add.
			*	@param globalZorder Global z order to set.
			*/
			bool addChild(Node* child, const int globalZOrder);

			/**
			*	Add child with global z order
			*	@param child Child node to add.
			*	@param zOrder Z order to set.
			*/
			bool addChild(Node* child, ZOrder& zOrder);

			/**
			*	Get child by name. This isn't efficient way since it uses string key.
			*	Use this for debug or incase where performance doesn't matter.
			*	@param name Name of ui.
			*	@return First ui child that matches the name. nullptr if doesn't exists
			*/
			Node* getChild(const std::string& name);

			/**
			*	Updates
			*	@param delta Elapsed time for current frame
			*/
			void update(const float delta);

			/**
			*	Run action.
			*/
			void runAction(Voxel::UI::Sequence* sequence);

			/**
			*	Render ui
			*/
			void render();

			/**
			*	Print. for debug
			*/
			void print(const int tab);
		};

		/**
		*	@class Canvas
		*	@brief A rectangular shape of area that defines screen space for UI.
		*/
		class Canvas : public Node
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
			// Constructor
			Canvas(const glm::vec2& size, const glm::vec2& centerPosition);
			
			// Size of ui space
			glm::vec2 size;

			// Center position of canvas in screen space. Center of monitor screen is 0
			glm::vec2 centerPosition;
			
			// UI screen matrix. 
			glm::mat4 uiScreenMatrix;
		public:
			// Destructor.
			~Canvas() = default;
			
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
			*	Render all UI
			*/
			void render();
		};

		/**
		*	@class Image
		*	@brief Rectangular png ui image that renders on screen space
		*
		*	Simple image that renders in screen space.
		*	Image will be static and won't be changed nor animated
		*	Uses 1 vao and 1 draw call
		*/
		class Image : public Node
		{
		private:
			/**
			*	Constructor
			*	@param name Name of ui
			*	@param imageName Name of image
			*/
			Image(const std::string& name);

			// Texture that image uses
			Texture2D* texture;

			// gl
			GLuint vao;

			/**
			*	Initialize image
			*	@param textureName Name of image texture file
			*	@return true if successfully creates ui image. Else, false.
			*/
			bool init(const std::string& textureName);

			/**
			*	Initialize image
			*	@param ss SpriteSheet instance.
			*	@param textureName Name of image texture file
			*	@return true if successfully creates ui image. Else, false.
			*/
			bool initFromSpriteSheet(SpriteSheet* ss, const std::string& textureName);

			/**
			*	Build image.
			*	Initialize vao.
			*	@param vertices Vertices of image quad
			*	@param colors Colors of image quad
			*	@param uvs Texture coordinates of image quad
			*	@param indices Indices of image quad
			*/
			void build(const std::vector<float>& vertices, const std::vector<float>& colors, const std::vector<float>& uvs, const std::vector<unsigned int>& indices);

		public:
			/**
			*	Creates image with single image file.
			*	If texture exists with same name, uses existing texture. Else, creates new texture.
			*	@param imageFileName Name of image file. 
			*	@return Image instance if successfully loads image and creates ui. Else, nullptr if anything fails.
			*/
			static Image* create(const std::string& name, std::string& imageFileName);

			/**
			*	Creates image from sprite sheet.
			*	@param imageFileName Name of image file.
			*	@param spriteSheetName Name of image file.
			*	@return Image instance if successfully loads image and creates ui. Else, nullptr if anything fails.
			*/
			static Image* createFromSpriteSheet(const std::string& name, const std::string& imageFileName, const std::string& spriteSheetName);

			// Destructor.
			~Image();

			/**
			*	Render image
			*	@param uiScreenMatrix A matrix that translates ui to ui screen space based on camera
			*	@param parentPi
			*/
			void render(const glm::mat4& uiScreenMatrix, const glm::mat4& canvasPivotMat);
		};


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
		*	@class Cursor
		*	@brief Textured cursor. Can switch to different cursors. Renders in screen space
		*
		*	Unlike other UIs, cursor doesn't have parent canvas. It's universal for all canvases. Therefore cursor is managed by Game class.
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

			void setPosition(const glm::vec2& position);
			void addPosition(const glm::vec2& distance);

			void updateBoundary();

			void setCursorType(const CursorType cursorType);

			void setVisibility(const bool visibility);

			glm::vec2 getPosition();
			glm::vec3 getWorldPosition();

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

			glm::vec4 color;
			glm::vec4 outlineColor;

			unsigned int indicesSize;
			unsigned int maxTextLength;
			
			float maxWidth;
			float totalHeight;
			
			ALIGN align;

			bool outlined;

			bool init(const std::string& text, const glm::vec2& position, const glm::vec4& color, ALIGN align = ALIGN::LEFT, TYPE type = TYPE::STATIC, const int maxLength = 0);
			bool initWithOutline(const std::string& text, const glm::vec2& position, const glm::vec4& color, const glm::vec4& outlineColor, ALIGN align = ALIGN::LEFT, TYPE type = TYPE::STATIC, const int maxLength = 0);
			bool buildMesh(const bool update);
			void loadBuffers(const std::vector<float>& vertices, const std::vector<float>& colors, const std::vector<float>& uvs, const std::vector<unsigned int>& indices);
			void updateBuffer(const std::vector<float>& vertices, const std::vector<float>& colors, const std::vector<float>& uvs, const std::vector<unsigned int>& indices);
			void clearBuffer();
			std::vector<glm::vec2> computeOrigins(Font* font, const std::vector<std::string>& split);
		public:
			~Text();

			static Text* create(const std::string& text, const glm::vec2& position, const glm::vec4& color, int fontID, ALIGN align = ALIGN::LEFT, TYPE type = TYPE::STATIC,  const int maxLength = 0);
			static Text* createWithOutline(const std::string& text, const glm::vec2& position, const int fontID, const glm::vec4& color, const glm::vec4& outlineColor, ALIGN align = ALIGN::LEFT, TYPE type = TYPE::STATIC, const int maxLength = 0);

			void setText(const std::string& text);
			std::string getText();

			bool isOutlined();

			void setColor(const glm::vec4& color);
			glm::vec4 getOutlineColor();

			void clear();

			void render(const glm::mat4& screenMat, const glm::mat4& canvasPivotMat, Program* prog);
		};

		/**
		*	@class Mesh
		*	@brief 3D object ui
		*/
		class Mesh : public UINode
		{
		private:
			glm::mat4 modelMat;
		};

		class Slider
		{

		};

		class Button
		{

		};
	}
}

#endif