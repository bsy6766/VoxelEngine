#ifndef TEXTURE_2D_H
#define TEXTURE_2D_H

#include <GL\glew.h>
#include <glm\glm.hpp>

namespace Voxel
{
	/**
	*	@class 
	*/
	class Texture2D
	{
	private:
		Texture2D();
	public:
		~Texture2D();

		static Texture2D* create();

		void bind(GLenum textureUnit);
	};
}

#endif