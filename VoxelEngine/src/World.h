#ifndef WORLD_H
#define WORLD_H

namespace Voxel
{
	// Foward
	class Chunk;
	class InputHandler;

	/**
	*	@class World
	*	@brief A world where everything is in. Terrain, player, weather, skybox, entity, items and all the stuffs that are rendered or even those doesen't.
	*
	*	For now, World instance is managed by Application class.
	*	There will be only one world at a time. There is no plan for zone phase shift or whatnot.
	*	Just giant single world where (theoretically) infinitely generates.
	*/
	class World
	{
	private:
	public:
		World();
		~World();

		Chunk* testChunk;
		InputHandler* input;
		float cameraMovementSpeed;

		float prevX, prevY;

		// Updates (tick) the world.
		void update(const float delta);
	};
}

#endif