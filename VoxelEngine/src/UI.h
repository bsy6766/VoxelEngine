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
#include <UIAction.h>

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
			// updates model matrix if it's true
			bool needToUpdateModelMat;

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

			// Parent of this node
			Node* parent;

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

			// Coordinate origin. This changes coordinate origin, which makes easier to place ui objects.
			glm::vec2 coordinateOrigin;
			
			// Model matrix
			glm::mat4 modelMat;
			
			// Size of original content
			glm::vec2 contentSize;

			// Bounding box
			Voxel::Shape::Rect boundingBox;

			// Action sequence
			Voxel::UI::Sequence* sequence;

			// Children
			Children children;

			// Program ptr
			Program* program;

			// Get contensize
			glm::vec2 getContentSize();

			// calculate model matrix
			virtual glm::mat4 getModelMatrix();
			
			// Updates model matrix based on parent's matrix
			virtual void updateModelMatrix();
		public:
			virtual ~Node();

			/**
			*	Get name
			*/
			std::string getName() const;

			/**
			*	Set visibility
			*	Setting visibility false will also affect all the children
			*	@parma visibility true to render this ui. false to not render.
			*/
			void setVisibility(const bool visibility);

			/**
			*1	Get visibility
			*/
			bool getVisibility() const;

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
			*	@param x x position in screen space
			*	@param y y position in screen space
			*/
			void setPosition(const float x, const float y);

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
			*	Set coordinate origin
			*	@param coordinateOrigin New coordinate origin to set. Default value is 0.0
			*/
			void setCoordinateOrigin(const glm::vec2& coordinateOrigin);

			/**
			*	Get coordinate origin
			*/
			glm::vec2 getCoordinateOrigin() const;

			/**
			*	Set bounding box
			*	@param center Center position of bounding box
			*	@param size Size of bounding box
			*/
			void setBoundingBox(const glm::vec2& center, const glm::vec2& size);

			/**
			*	Get bounding box. 
			*/
			Voxel::Shape::Rect getBoundingBox() const;

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
			*	Check if this node has children
			*	@return true if has children. Else, false.
			*/
			bool hasChildren();

			/**
			*	Converts children map to vector.
			*	This will search for neested childrens recursively and add up all.
			*/
			void getAllChildrenInVector(std::vector<Node*>& nodes, Node* parent);

			/**
			*	Updates
			*	@param delta Elapsed time for current frame
			*/
			virtual void update(const float delta);

			/**
			*	Run action.
			*/
			void runAction(Voxel::UI::Sequence* sequence);

			/**
			*	Render self. Pure virtual. All UI need to override this to render itself during the ui graph traversal
			*/
			virtual void renderSelf() = 0;

			/**
			*	Render self and children
			*/
			virtual void render();

			// Debug print
			virtual void print(const int tab);
			void printChildren(const int tab);
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
			
			// Size of ui space
			glm::vec2 size;

			// Center position of canvas in screen space. Center of monitor screen is 0
			glm::vec2 centerPosition;
			
			// Override
			void updateModelMatrix() override;

			// override
			glm::mat4 getModelMatrix() override;
		public:
			// Constructor
			Canvas(const glm::vec2& size, const glm::vec2& centerPosition);

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
			*	Render self
			*/
			void renderSelf() override;

			/**
			*	Render all UI
			*/
			void render() override;

			// print
			void print(const int tab) override;
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
			Image() = delete;

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
			unsigned int indicesSize;

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
			// Destructor.
			~Image();

			/**
			*	Creates image with single image file.
			*	If texture exists with same name, uses existing texture. Else, creates new texture.
			*	@param imageFileName Name of image file. 
			*	@return Image instance if successfully loads image and creates ui. Else, nullptr if anything fails.
			*/
			static Image* create(const std::string& name, std::string& imageFileName);

			/**
			*	Creates image from sprite sheet.
			*	@param spriteSheetName Name of image file.
			*	@param imageFileName Name of image file.
			*	@return Image instance if successfully loads image and creates ui. Else, nullptr if anything fails.
			*/
			static Image* createFromSpriteSheet(const std::string& name, const std::string& spriteSheetName, const std::string& imageFileName);

			/**
			*	Render self
			*/
			void renderSelf() override;
			
#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX
			// gl
			GLuint bbVao;
#endif
		};

		class AnimatedImage : public Node
		{
		private:
			AnimatedImage() = delete;

			// Constructor
			AnimatedImage(const std::string& name);

			// Frame size of animation. Must be positive number
			int frameSize;

			// Interval between frames.
			float interval;

			// Currently elapsed time to keep track interval.
			float elapsedTime;

			// Current frame index. Starts from 0.
			int currentFrameIndex;

			// Texture (spritesheet)
			Texture2D* texture;

			// gl
			GLuint vao;
			unsigned int indicesSize;
			unsigned int currentIndex;

			// Repeats animation if this is true. Else, stops on last frame.
			bool repeat;

			// If animation is stopped, can't be paused or resumed. Must call start() to run animation from the beginning.
			bool stopped;

			// If animation is paused, then it can either call start() to start over animation or call resume() to resume animation.
			bool paused;

			/**
			*	Initialize animated image
			*	@param ss SpriteSheet pointer
			*	@param frameName Image frame file name.
			*	@param frameSize Size of frame. Must be greater than 0.
			*	@param interval Interval between frame.
			*	@param repeat true if animation repeats. Else false.
			*/
			bool init(SpriteSheet* ss, const std::string& frameName, const int frameSize, const float interval, const bool repeat);
		public:
			/**
			*	Initialize animated image
			*	@param spriteSheetName Name of sprite sheet that has image frames. All frames must be in same sprite sheet.
			*	@param frameName Image frame file name.
			*	@param frameSize Size of frame. Must be greater than 0.
			*	@param interval Interval between frame. Must be greater than 0.
			*	@param repeat true if animation repeats. Else false.
			*/
			static AnimatedImage* create(const std::string& name, const std::string& spriteSheetName, const std::string& frameName, const int frameSize, const float interval, const bool repeat);

			/**
			*	Build image.
			*	Initialize vao.
			*	@param vertices Vertices of image quad
			*	@param colors Colors of image quad
			*	@param uvs Texture coordinates of image quad
			*	@param indices Indices of image quad
			*/
			void build(const std::vector<float>& vertices, const std::vector<float>& colors, const std::vector<float>& uvs, const std::vector<unsigned int>& indices);

			/**
			*	Start animation from the first frame
			*/
			void start();

			/**
			*	Pause animation
			*/
			void pause();

			/**
			*	Resume animation
			*/
			void resume();

			/**
			*	Stop animation. 
			*/
			void stop();

			/**
			*	Set interval. Must be greater than 0.
			*/
			void setInterval(const float interval);

			/**
			*	Override bases update function 
			*/
			void update(const float delta) override;

			/**
			*	Render self
			*/
			void renderSelf() override;
		};

		/**
		*	@class Text
		*	@brief List of quads where each quad renders each character.
		*
		*	Text class builds list of quads one by one, based on font metric.
		*	It doesn't provide any font related function, such as linespace or size modification.
		*	Use FontManager to get font and modify values from there.
		*/
		class Text : public Node
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
			GLuint vao;
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
			
#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX
			// gl
			GLuint bbVao;
#endif
		};
		
		/**
		*	@class Mesh
		*	@brief 3D object ui
		*/
		class Mesh : public Node
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