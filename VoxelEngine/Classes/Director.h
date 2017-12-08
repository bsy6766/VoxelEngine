#ifndef DIRECTOR_H
#define DIRECTOR_H

namespace Voxel
{
	/**
	*	@class Director
	*	@brief A director that decides which to update and which to render.
	*
	*	Director manages multiple scenes in the game.
	*	Like real director, it directs update and render to specific scene.
	*	Scene can be title scene, menu scene, etc.
	*	
	*	
	*/
	class Director
	{
	public:
		enum class Scene
		{
			TITLE_SCENE = 0,
			GAME_SCENE,
		};
	public:
		// Constructor
		Director();

		// Destructor
		~Director();
	};

}
#endif