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
#include <map>
#include <list>

// Voxel
#include "ZOrder.h"
#include "Shape.h"

namespace Voxel
{
	// Forward declarations
	class Texture2D;
	class Font;
	class Program;

	namespace UI
	{
		// Forward declaration
		class Node;
		class TransformNode;
		class RenderNode;
		class Action;

		typedef std::map<ZOrder, TransformNode*, ZOrderComp> Children;

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
		*	Even though TransformNode itself doesn't renders on screen, 
		*	but derived classes can, that is why it has pure virtual render function, opacity and visibility.
		*/
		class TransformNode : public Node
		{
		public:
			enum InteractionFlag
			{
				NONE = 0,				// Not interactable, not draggable
				INTERACTABLE = 1,		// Interactable. Getss mouse input feed
				DRAGGABLE = 2,			// Draggable. Pan around screenspace while clicked.
			};
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

			// interaction flag
			unsigned int interaction;

			// Visibility. true if visible. false if invisible and ignores opaicty
			bool visibility;

			// Opacity
			float opacity;

			// Z order of ui
			ZOrder zOrder;

			// Position
			glm::vec2 position;

			// Rotation in z axis. range [-360.0f, 360.0f]
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
			std::list<Voxel::UI::Action*> actions;

			// Action pause. 
			bool actionPaused;

			// get parent matrix
			glm::mat4 getParentMatrix() const;

			// calculate model matrix
			virtual glm::mat4 getModelMatrix();
		public:
			// Destructor
			virtual ~TransformNode();

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
			*	Add position
			*/
			void addPosition(const glm::vec2& delta);

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
			*	Set scale of ui
			*	@param scale SCale of ui for both x and y axis.
			*/
			void setScale(const float scale);

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
			*	Get content size of ui. This is the original size of content without any transformation applied.
			*/
			glm::vec2 getContentSize() const;

			/**
			*	Set bounding box
			*	@param center Center position of bounding box
			*	@param size Size of bounding box
			*/
			void setBoundingBox(const glm::vec2& center, const glm::vec2& size);

			/**
			*	Get bounding box.
			*/
			virtual Voxel::Shape::Rect getBoundingBox() const;

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
			*	Set ui interactable
			*/
			void setInteractable();

			/**
			*	Set ui non-interactable
			*/
			void setNonInteractable();

			/**
			*	Set ui draggable
			*/
			void setDraggable();
			
			/**
			*	Set ui undraggable
			*/
			void setUndraggable();

			/**
			*	Check if ui is interacble
			*/
			bool isInteractable() const;

			/**
			*	Check if ui is draggable
			*/
			bool isDraggable() const;

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
			*	Remove child by name. Searches the name and remvoes from children.
			*/
			bool removeChild(const std::string& name, const bool releaseChild = false);

			/**
			*	Remove child by id. Searches the id and remvoes from children.
			*/
			bool removeChild(const unsigned int id, const bool releaseChild = false);

			/**
			*	Remove child by instance. 
			*	This attemps to find by z order. If fails, searcehs with id.
			*/
			bool removeChild(TransformNode* child, const bool releaseChild = false);

			/**
			*	Set parent.
			*	@param parent A parent ui to set. If parent is nullptr, does nothing. To remove parent @see removeParent()
			*/
			void setParent(TransformNode* parent);

			/**
			*	Removes parent and removes itself from parent.
			*/
			void removeParent();

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
			*	Updates model matrix.
			*	This will update model marix based on ui's tranformation and parent's matrix.
			*	This function will update model matrix itself and all children aswell and it's called whenever transformation (translation, rotation, scale, pivot, etc) changes on self or any parents to root parent.
			*/
			virtual void updateModelMatrix();

			/**
			*	Update model matrix with parent matrix
			*	This will update model matrix with given parent matrix instead of actual parent.
			*	This is useful when trying to treat ui as child of other ui without actually adding it as child.
			*/
			virtual void updateModelMatrix(const glm::mat4& parentMatrix);

			/**
			*	Updates bounding box
			*	This will update the bounding box to screen space, not node space.
			*	This function will only update bounding box itself but children. Also it's called whenever transformation (translation, rotation, scale, pivot, etc) changes on self or any parents to root parent.
			*/
			virtual void updateBoundingBox();

			/**
			*	Update bounding box with parent matrix
			*	this will update bounding box with given parent matrix instead of actual parent.
			*	This is usefull when trying to tread ui ad child of ther ui without actually adding it as child
			*/
			virtual void updateBoundingBox(const glm::mat4& parentMatrix);

			/**
			*	Update keyboard update
			*	Some ui like inputfield need to update the text as user types in.
			*	@param str A string of buffer that is stacked up in inputhandler
			*/
			virtual bool updateKeyboardInput(const std::string& str);

			/**
			*	Update mouse movement.
			*	@param mosuePosition Current position of mouse in screen space
			*	@param mouseDelta Amount of mouse moved.
			*/
			virtual bool updateMouseMove(const glm::vec2& mousePosition, const glm::vec2& mouseDelta);

			/**
			*	update mouse click
			*	@param mousePosition Current position of mouse in screen space
			*	@param button Clciked mouse button. 0 = left, 1 = right, 2 = middle
			*/
			virtual bool updateMousePress(const glm::vec2& mousePosition, const int button);

			/**
			*	update mouse release
			*	@param mousePosition Current position of mouse in screen space
			*	@param button Released mouse button. 0 = left, 1 = right, 2 = middle
			*/
			virtual bool updateMouseRelease(const glm::vec2& mousePosition, const int button);

			/**
			*	Update ui boundary. Checks if ui is out of canvas screen
			*/
			virtual void updateBoundary(const Voxel::Shape::Rect& canvasBoundary);

			/**
			*	Run action.
			*/
			void runAction(Voxel::UI::Action* action);

			/**
			*	Pause action
			*/
			void pauseAction();

			/**
			*	Resume action
			*/
			void resumeAction();

			/**
			*	Restart action
			*/
			void restartAllActions();

			/**
			*	Stops all actions and release it
			*/
			void stopAllActions();

			// Debug print
			virtual void print(const int tab);
			void printChildren(const int tab);

			// render
			virtual void render() = 0;

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX
			// gl
			GLuint bbVao;
			void createDebugBoundingBoxLine();
#endif
		};

		/**
		*	@class RenderNode
		*	@brief A node that can be rendered. Derived from TransfomNode. Used by different UI classes.
		*/
		class RenderNode : public TransformNode
		{
		protected:
			// Constructor
			RenderNode() = delete;
			RenderNode(const std::string& name);
			
			// Program ptr
			Program* program;

			// gl 
			GLuint vao;

			// Texture that image uses
			Texture2D* texture;
			
			// Color of object
			glm::vec3 color;
		public:
			virtual ~RenderNode();

			/**
			*	Set color. 
			*	@color Color to apply. Color is multiplied with texture. White by default. [0.0f, 1.0f]
			*/
			void setColor(const glm::vec3& color);

			/**
			*	Get color
			*/
			glm::vec3 getColor() const;
			
			/**
			*	Render self. Pure virtual. All UI need to override this to render itself during the ui graph traversal
			*/
			virtual void renderSelf() = 0;

			/**
			*	Render self and children
			*/
			void render() override;
		};
	}
}

#endif