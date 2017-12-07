#ifndef MOVE_TO_H
#define MOVE_TO_H

// Voxel
#include "Action.h"

namespace Voxel
{
	namespace UI
	{
		// forward declaration
		class TransformNode;

		/**
		*	@class MoveTo
		*	@brief Move ui to specific position
		*/
		class MoveTo : public Action
		{
		private:
			// Constructor
			MoveTo();

			// destination
			glm::vec2 destination;

			// target. Need TransformNode at least.
			TransformNode* target;

			// init
			bool init(const float duration, const glm::vec2& destination);
		public:
			// Destructor
			~MoveTo() = default;

			/**
			*	Creates MoveTo action
			*	@param duration Duration of action
			*	@param destination Position to move
			*/
			static MoveTo* create(const float duration, const glm::vec2& destination);

			// override
			void setTarget(TransformNode* target);

			// ovrrides
			void update(const float delta) override;
		};
	}
}

#endif