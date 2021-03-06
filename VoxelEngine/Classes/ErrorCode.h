#ifndef ERROR_CODE_H
#define ERROR_CODE_H

#include <string>

namespace Voxel
{
	namespace Error
	{
		/**
		*	Simple error code header.
		*	This error code is
		*/
		enum Code : unsigned int
		{
			ERROR_NONE = 0,										// 0. No Error
			ERROR_FAILED_TO_INITIALIZE_GLFW,
			ERROR_FAILED_TO_CREATE_GLFW_WINDOW,	
			ERROR_FAILED_TO_INITIALIZE_GLEW,
			ERROR_FAILED_TO_OPEN_SHADER_FILE,
			ERROR_SHADER_FILE_IS_EMPTY,
			ERROR_SHADER_COMPILE_ERROR,
			ERROR_NULL_VERTEX_SHADER,
			ERROR_NULL_FRAGMENT_SHADER,
			ERROR_FAILED_TO_CREATE_GL_PROGRAM,
			ERROR_FAILED_TO_LINK_PROGRAM,
			ERROR_ATTRIBUTE_NAME_IS_EMPTY,
			ERROR_ATTRIBUTE_NAME_DOES_NOT_EXISTS,
			ERROR_UNIFORM_NAME_IS_EMPTY,
			ERROR_UNIFORM_DOES_NOT_EXISTS,
			ERROR_FAILED_TO_INITIALIZE_FREETYPE2,
			ERROR_FAILED_TO_LOAD_FONT_FILE,
			ERROR_FONT_TEXTURE_TOO_SMALL,					// Font failed to generate texture because texture was too small
			ERROR_FAILED_TO_INITIALIZE_SDL,

		};
	}
}

#endif