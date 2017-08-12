#ifndef PROGRAM_H
#define PROGRAM_H

#include <GL\glew.h>

namespace Voxel
{
	class Shader;

	/**
	*	@class Program
	*	@brief OpenGL Program instance
	*/
	class Program
	{
	protected:
		Program() = default;
	private:
		GLuint programObject;

		bool init(Shader* vertexShader, Shader* fragmentShader);

		void checkLinkError();
	public:
		~Program();
		static Program* create(Shader* vertexShader, Shader* fragmentShader);

		GLuint getObject();

		GLint attrib(const GLchar* attributeName);
	};
}

#endif PROGRAM_H
