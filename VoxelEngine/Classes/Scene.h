#ifndef SCENE_H
#define SCENE_H

namespace Voxel
{
	/**
	*	@class Scene
	*	@brief Base class for all scene.
	*
	*	This class doesn't have any data or logic.
	*	It just provides interface for all scene. Init, Update and Render.
	*/
	class Scene
	{
	private:
	protected:
		// Constructor
		Scene() = default;
	public:
		// Destructor
		virtual ~Scene() = default;

		// Init. Called when scene is created by Director
		virtual void init() = 0;

		// Called when scene enters.
		virtual void onEnter() = 0;

		// Called when scene exits. This is called before the destructor of derived class.
		virtual void onExit() = 0;

		// Updates scene
		virtual void update(const float delta) = 0;

		// Renders scene
		virtual void render() = 0;
	};
}

#endif