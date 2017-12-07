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
		*	@brief Base class for all actions and sequence.
		*/
		class Action
		{
		protected:
			// Constructor
			Action();

			// duration of action
			float duration;

			// elapsed time
			float elapsedTime;

		public:
			// Destructor
			virtual ~Action() = default;

			// Check if action is done
			bool isDone();

			// Get exceeded time 
			float getExceededTime();

			// set target. Derived class override this
			virtual void setTarget(TransformNode* target);
			
			// reset
			void reset();

			// Update action
			virtual void update(const float delta) = 0;
		};		
	}
}

#endif