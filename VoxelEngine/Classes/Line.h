#ifndef LINE_H
#define LINE_H

// voxel
#include "RenderNode.h"

namespace Voxel
{
	// foward declaration
	class Program;

	namespace UI
	{
		/**
		*	@class Line
		*	@brief Renders line on ui sapce. Created for debug purpose.
		*/
		class Line : public TransformNode
		{
		private:
			Line() = delete;
			Line(const std::string& name);

			// gl
			GLuint vao;

			// program
			Program* program;

			// color
			glm::vec4 lineColor;

			// init
			bool init(const glm::vec2& start, const glm::vec2& end, const glm::vec4& lineColor);
		public:
			// Destructor
			~Line();

			// create
			static Line* create(const std::string& name, const glm::vec2& start, const glm::vec2& end, const glm::vec4& lineColor);

			// set color
			void setColor(const glm::vec4& lineColor);

			// render self
			void renderSelf();

			// render
			void render() override;
		};
	}
}

#endif