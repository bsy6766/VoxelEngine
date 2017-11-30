#ifndef UI_H
#define UI_H

// Config
#include "Config.h"

// glm
#include <glm/glm.hpp>

// gl
#include <GL\glew.h>

// cpp
#include <string>
#include <unordered_map>
#include <map>
#include <memory>
#include <array>

// Voxel
#include "ZOrder.h"
#include "Shape.h"
#include "UIAction.h"

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
		class TransformNode;
		class RenderNode;
		class Image;
		class AnimatedImage;
		class Text;
		class Button;
		class ProgressTimer;

		typedef std::map<ZOrder, std::unique_ptr<TransformNode>, ZOrderComp> Children;

		/**
		*	@class Node
		*	@brief Base class of all UI component
		*	
		*	Node is simple base class that has id number and name. 
		*	Node itself does nothing and can't be created as instance.
		*/
		class Node
		{
		private:
			static unsigned int idCounter;
		protected:
			// Constructor
			Node() = delete;
			Node(const std::string& name);

			// name
			std::string name;

			// id
			unsigned int id;
		public:
			// Destructor
			~Node();

			/**
			*	Get number id of ui
			*/
			unsigned int getID() const;

			/**
			*	Get name of ui
			*/
			std::string getName() const;
		};

		/**
		*	@class TransformNode
		*	@brief A node that can be transformed. Derived from Node class.
		*
		*	TransfromNode has own model mat and can have parent and children.
		*/
		class TransformNode : public Node
		{
		private:
			// updates model matrix if it's true
			bool needToUpdateModelMat;

			// Find next ZOrder
			bool getNextZOrder(ZOrder& curZOrder);
		protected:
			// constructor
			TransformNode() = delete;
			TransformNode(const std::string& name);

			// Parent
			TransformNode* parent;

			// Children
			Children children;

			// Z order of ui
			ZOrder zOrder;

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

			// Get contensize
			glm::vec2 getContentSize();

			// calculate model matrix
			virtual glm::mat4 getModelMatrix();

			// Updates model matrix based on parent's matrix
			virtual void updateModelMatrix();
		public:
			// Destructor
			virtual ~TransformNode();

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
			bool addChild(TransformNode* child);

			/**
			*	Add child with global z order
			*	@param child Child node to add.
			*	@param globalZorder Global z order to set.
			*/
			bool addChild(TransformNode* child, const int globalZOrder);

			/**
			*	Add child with global z order
			*	@param child Child node to add.
			*	@param zOrder Z order to set.
			*/
			bool addChild(TransformNode* child, ZOrder& zOrder);

			/**
			*	Get child by name. This isn't efficient way since it uses string key.
			*	Use this for debug or incase where performance doesn't matter.
			*	@param name Name of ui.
			*	@return First ui child that matches the name. nullptr if doesn't exists
			*/
			TransformNode* getChild(const std::string& name);

			/**
			*	Check if this node has children
			*	@return true if has children. Else, false.
			*/
			bool hasChildren();

			/**
			*	Converts children map to vector.
			*	This will search for neested childrens recursively and add up all.
			*/
			void getAllChildrenInVector(std::vector<TransformNode*>& nodes, TransformNode* parent);

			/**
			*	Updates
			*	@param delta Elapsed time for current frame
			*/
			virtual void update(const float delta);

			/**
			*	Update mouse movement.
			*	@param mosuePosition Current position of mouse in screen space
			*/
			virtual void updateMouseMove(const glm::vec2& mousePosition);

			/**
			*	update mouse click
			*	@param mousePosition Current position of mouse in screen space
			*	@param button Clciked mouse button. 0 = left, 1 = right, 2 = middle
			*/
			virtual void updateMouseClick(const glm::vec2& mousePosition, const int button);

			/**
			*	update mouse release
			*	@param mousePosition Current position of mouse in screen space
			*	@param button Released mouse button. 0 = left, 1 = right, 2 = middle
			*/
			virtual void updateMouseRelease(const glm::vec2& mousePosition, const int button);

			/**
			*	Run action.
			*/
			void runAction(Voxel::UI::Sequence* sequence);

			// Debug print
			virtual void print(const int tab);
			void printChildren(const int tab);

			// render
			virtual void render() = 0;
		};

		/**
		*	@class RenderNode
		*	@brief A node that can be rendered. Derived from TransfomNode
		*/
		class RenderNode : public TransformNode
		{
		protected:
			// Constructor
			RenderNode() = delete;
			RenderNode(const std::string& name);
			
			// Visibility. true if visible. false if invisible and ignores opaicty
			bool visibility;

			// Opacity
			float opacity;
			
			// Program ptr
			Program* program;

			// gl 
			GLuint vao;

			// Texture that image uses
			Texture2D* texture;
			
			// Color of object
			glm::vec3 color;

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX
			// gl
			GLuint bbVao;
			void createDebugBoundingBoxLine();
#endif
		public:
			virtual ~RenderNode();
			
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
			*	Render self. Pure virtual. All UI need to override this to render itself during the ui graph traversal
			*/
			virtual void renderSelf() = 0;

			/**
			*	Render self and children
			*/
			void render() override;
		};

		/**
		*	@class Canvas
		*	@brief A rectangular shape of area that defines screen space for UI.
		*/
		class Canvas : public TransformNode
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

			// Visibility. true if visible. false if invisible and ignores opaicty
			bool visibility;
			
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
			*	Set size of canvas
			*	@param size Size of canvas. Must be positive numbers
			*/
			void setSize(const glm::vec2& size);

			/**
			*	Update all UI
			*/
			void update(const float delta);

			/**
			*	Update mouse movement.
			*	@param mosuePosition Current position of mouse in screen space
			*/
			void updateMouseMove(const glm::vec2& mousePosition);

			/**
			*	update mouse click
			*	@param mousePosition Current position of mouse in screen space
			*	@param button Clciked mouse button. 0 = left, 1 = right, 2 = middle
			*/
			void updateMouseClick(const glm::vec2& mousePosition, const int button);

			/**
			*	update mouse released
			*	@param mousePosition Current position of mouse in screen space
			*	@param button Release mouse button. 0 = left, 1 = right, 2 = middle
			*/
			void updateMouseRelease(const glm::vec2& mousePosition, const int button);
			
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
		class Image : public RenderNode
		{
		private:
			Image() = delete;

			/**
			*	Constructor
			*	@param name Name of ui
			*	@param imageName Name of image
			*/
			Image(const std::string& name);
			
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
			*	@param vertices Vertices of image quad. Single image has 12 vertices.
			*	@param uvs Texture coordinates of image quad. Single image has 8 uv coordinates
			*	@param indices Indices of image quad. Single image has 6 indices
			*/
			virtual void build(const std::array<float, 12>& vertices, const std::array<float, 8>& uvs, const std::array<unsigned int, 6>& indices);
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
		};

		/**
		*	@class AnimatedImage
		*	@brief A series of image frame that animates with interval.
		*
		*	Similar to image but have multiple image frames that animates within given interval.
		*	Animated image is static and won't be able to modify once created.
		*	Yet, can change few attributes such as interval, repeat, etc.
		*
		*	Image frame format must be ImageFrameName_FrameNumber.png, where ImageFrameName is image file name and FrameNumber is frame number.
		*	For example, image file name 'player_anim" and 3 frames will be 'player_anim_0.png', 'player_anim_1.png" and 'player_anim_2.png'.
		*/
		class AnimatedImage : public RenderNode
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
			
			// Size of rames
			std::vector<glm::vec2> frameSizes;

			// gl
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

			/**
			*	Build image.
			*	Can't use array because frame size is dynamic
			*	@param vertices Vertices of image quad. 
			*	@param uvs Texture coordinates of image 
			*	@param indices Indices of image quad.
			*/
			virtual void build(const std::vector<float>& vertices, const std::vector<float>& uvs, const std::vector<unsigned int>& indices);
		public:
			// Desturctor
			~AnimatedImage() = default;

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

		/**
		*	@class Button
		*	@brief A simple button that can be clicked
		*/
		class Button : public RenderNode
		{
		public:
			enum class State
			{
				IDLE = 0,
				HOVERED,
				CLICKED,
				DISABLED
			};
		private:
			Button() = delete;

			// constructor with name
			Button(const std::string& name);

			// Button state. IDLE is default
			State buttonState;
			
			// frame sizes incase button images are different
			std::array<glm::vec2, 4> frameSizes;

			// gl
			unsigned int currentIndex;

			/**
			*	Initialize button
			*/
			bool init(SpriteSheet* ss, const std::string& buttonImageFileName);

			/**
			*	Build image.
			*	@param vertices Vertices of image quad.
			*	@param uvs Texture coordinates of image
			*	@param indices Indices of image quad.
			*/
			virtual void build(const std::vector<float>& vertices, const std::vector<float>& uvs, const std::vector<unsigned int>& indices);
		public:
			// Destructor
			~Button() = default;

			/**
			*	Create button.
			*	@param name Name of button ui
			*	@param spriteSheetName Name of sprite sheet that has button images.
			*	@param buttonImageFileName Image file name of button. Button requires total 4 (idle, hovered, clicked, disabled) images. 
			*	buttomImageFileName will be used to load all 4 required iamges by appending '_TYPE' at the end of buttonImageFileName.
			*	For example, buttonImageFileName 'menu_button' or 'menu_button.png' will load 'menu_button_idle.png', 'menu_button_hovered.png',
			*	'menu_button_clicked.png' and 'menu_button_disabled.png'. 
			*	All images must be in same sprite sheet.
			*/
			static Button* create(const std::string& name, const std::string& spriteSheetName, const std::string& buttonImageFileName);

			/**
			*	Enable button.
			*/
			void enable();

			/**
			*	Disable button
			*/
			void disable();

			/**
			*	Check if mouse is hovering button
			*/
			void updateMouseMove(const glm::vec2& mousePosition) override;

			/**
			*	Check if mouse clicked the button
			*/
			void updateMouseClick(const glm::vec2& mousePosition, const int button) override;

			/**
			*	Check if mouse released the button
			*/
			void updateMouseRelease(const glm::vec2& mousePosition, const int button) override;

			/**
			*	Render self
			*/
			void renderSelf() override;
		};

		/**
		*	@class CheckBox
		*	@brief A simple checkbox that can be checked or unchekced
		*/
		class CheckBox : public RenderNode
		{
		public:
			enum class State
			{
				DESELECTED = 0,
				HOVERED,
				CLICKED,
				SELECTED,
				HOVERED_SELECTED,
				CLICKED_SELECTED,
				DISABLED
			};
		private:
			// Constructor
			CheckBox() = delete;
			CheckBox(const std::string& name);

			// state
			State prevCheckBoxState;
			State checkBoxState;

			// gl
			unsigned int currentIndex;

			/**
			*	Initialize button
			*/
			bool init(SpriteSheet* ss, const std::string& checkBoxImageFileName);

			/**
			*	Build image.
			*	Initialize vao.
			*	@param vertices Vertices of image quad
			*	@param uvs Texture coordinates of image quad
			*	@param indices Indices of image quad
			*/
			void build(const std::vector<float>& vertices, const std::vector<float>& uvs, const std::vector<unsigned int>& indices);

			/**
			*	Update current index based on state
			*/
			void updateCurrentIndex();
		public:
			// Destructor
			~CheckBox() = default;

			/**
			*	Create checkbox
			*	@param name Name of ui
			*	@param spriteSheetName Name of sprite sheet that has check box ui images
			*	@param checkBoxImageFileName Image file name of check box. This will used to load check box ui images
			*/
			static CheckBox* create(const std::string& name, const std::string& spriteSheetName, const std::string& checkBoxImageFileName);

			/**
			*	Enable button.
			*/
			void enable();

			/**
			*	Disable button
			*/
			void disable();

			/**
			*	Select check box manually. Ignored when check box is disabled.
			*/
			void select();

			/**
			*	Deselect check box manually. Ignored when check box is disabled.
			*/
			void deselect();

			/**
			*	Check if mouse is hovering button
			*/
			void updateMouseMove(const glm::vec2& mousePosition) override;

			/**
			*	Check if mouse clicked the button
			*/
			void updateMouseClick(const glm::vec2& mousePosition, const int button) override;

			/**
			*	Check if mouse released the button
			*/
			void updateMouseRelease(const glm::vec2& mousePosition, const int button) override;

			/**
			*	Render self
			*/
			void renderSelf() override;
		};

		/**
		*	@class ProgressTimer
		*	@brief A simple progress timer. Can be BAR type or RADIAL type.
		*/
		class ProgressTimer : public RenderNode
		{
		public:
			enum class Type
			{
				HORIZONTAL = 0,
				VERTICAL,
				RADIAL
			};

			enum class Direction
			{
				CLOCK_WISE = 0,
				COUNTER_CLOCK_WISE
			};
		private:
			// Constructor
			ProgressTimer() = delete;
			ProgressTimer(const std::string& name);
			
			// percenatge. 0 ~ 100
			int percentage;
			
			// current index
			int currentIndex;

			// type of progress bar
			Type type;

			/**
			*	Initialize button
			*/
			bool init(SpriteSheet* ss, const std::string& progressTimerImageFileName, const Type type = Type::HORIZONTAL, const Direction direction = Direction::CLOCK_WISE);
			
			void buildMesh(const glm::vec2& verticesOrigin, const glm::vec2& verticesEnd, const glm::vec2& uvOrigin, const glm::vec2& uvEnd, std::vector<float>& vertices, std::vector<float>& uvs, std::vector<unsigned int>& indices, const Direction direction);

			/**
			*	Build image.
			*	Initialize vao.
			*	@param vertices Vertices of image quad
			*	@param uvs Texture coordinates of image quad
			*	@param indices Indices of image quad
			*/
			void build(const std::vector<float>& vertices, const std::vector<float>& uvs, const std::vector<unsigned int>& indices);

			/**
			*	Updates current index based on progress timer.
			*	Background, percentage and type affects index.
			*/
			void updateCurrentIndex();
		public:
			// Desturctor
			~ProgressTimer() = default;

			/**
			*	Create progress timer
			*/
			static ProgressTimer* create(const std::string& name, const std::string& spriteSheetName, const std::string& progressTimerImageFileName, const Type type = Type::HORIZONTAL, const Direction direction = Direction::CLOCK_WISE);

			/**
			*	Set percentage. 
			*	@param precentage. Must be 0 ~ 100
			*/
			void setPercentage(const int percentage);

			/**
			*	Get percentage
			*/
			int getPercentage() const;

			/**
			*	Render self
			*/
			void renderSelf() override;
		};
		
		/**
		*	@class Mesh
		*	@brief 3D object ui
		*/
		class Mesh : public RenderNode
		{
		private:
			glm::mat4 modelMat;
		};

		class Slider
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