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
			ERROR_FAILED_TO_INITIALIZE_GLFW = 1,				// 1
			ERROR_FAILED_TO_CREATE_GLFW_WINDOW = 2,				// 2
			ERROR_FAILED_TO_INITIALIZE_GLEW = 3,				// 3
			ERROR_FAILED_TO_OPEN_SHADER_FILE = 4,				// 4
			ERROR_SHADER_FILE_IS_EMPTY = 5,
			ERROR_SHADER_COMPILE_ERROR = 6,
			ERROR_NULL_VERTEX_SHADER = 7,
			ERROR_NULL_FRAGMENT_SHADER = 8,
			ERROR_FAILED_TO_CREATE_GL_PROGRAM = 9,
			ERROR_FAILED_TO_LINK_PROGRAM = 10,
			ERROR_FAILED_TO_INITIALIZE_FREETYPE2 = 11,
			ERROR_FAILED_TO_LOAD_FONT_FILE = 12,
			ERROR_FONT_TEXTURE_TOO_SMALL = 13,					// Font failed to generate texture because texture was too small
			ERROR_FAILED_TO_INITIALIZE_SDL = 14,

		};
	}
}

#endif