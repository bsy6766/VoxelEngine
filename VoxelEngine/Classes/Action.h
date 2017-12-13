#ifndef ACTION_H
#define ACTION_H

// glm
#include <glm\glm.hpp>

namespace Voxel
{
	namespace UI
	{
		// foward declaration
		class TransformNode;

		/**
		*	@class Action
		*	@brief An action for UI. Can apply various actions such as movement, rotation, fade and more to ui object.
		*
		*	Action is base class for all other ui action classes including decorator like sequence, repeat, etc.
		*/
		class Action
		{
		protected:
			// Constructor
			Action();

			// Total duration of this action
			float duration;

			// Elapsed time for this action
			float elapsedTime;
		public:
			// Default destructor.
			virtual ~Action() = default;

			// Check if action is done. Action is done if time is up (elapsedTime >= duration)
			virtual bool isDone() const;

			// Get exceeded time.
			float getExceededTime() const ;

			// Get duration
			float getDuration() const;

			// Get elapsedTime
			float getElapsedTime() const;

			// Reset action
			virtual void reset();

			// set target. Derived classes override this to set their own target. Some actions doesn't have target so it's up to derived class to update. Base class setTarget does nothing.
			virtual void setTarget(TransformNode* target);

			// Update action. All derived actions must override
			virtual void update(const float delta) = 0;
		};	
	}
}

#endif