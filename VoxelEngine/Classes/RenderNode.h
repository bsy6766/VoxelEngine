#ifndef RENDER_NODE_H
#define RENDER_NODE_H

// voxel
#include "TransformNode.h"

// gl
#include <GL\glew.h>

namespace Voxel
{
	// foward declaration
	class Program;
	class Texture2D;

	namespace UI
	{
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