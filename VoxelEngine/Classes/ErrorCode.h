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
			NO_ERROR = 0,							// 0. No Error
			FAILED_TO_INITIALIZE_GLFW = 1,			// 1. Failed to initialize glfw
			FAILED_TO_CREATE_GLFW_WINDOW = 2,		// 2. Failed to create glfw window
			FAILED_TO_INITIALIZE_GLEW = 3,			// 3. 
		};
	}
}

#endif