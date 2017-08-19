#ifndef SPRITE_H
#define SPRITE_H

namespace Voxel
{
	// Forward
	class Texture;

	/**
	*	@class Sprite
	*	@brief Rectangular textured quad renders in world space. Use UI::Image for screen space.
	*/
	class Sprite
	{
	private:
		Sprite();

		// Texture that this sprite uses
	public:
		~Sprite();
	};
}

#endif