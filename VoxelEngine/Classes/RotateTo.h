#ifndef ROTATE_TO_H
#define ROTATE_TO_H

// Voxel
#include "Action.h"

namespace Voxel
{
	namespace UI
	{
		// forward declaration
		class TransformNode;

		/**
		*	@class RotateTo
		*	@brief Rotate ui to specific angle
		*/
		class RotateTo : public Action
		{
		private:
			// Constructor
			RotateTo();

			// angle
			float angle;

			// target. Need TransformNode at least.
			TransformNode* target;

			// init
			bool init(const float duration, const float angle);

			// wrap angle to [-360, 360]
			void wrapAngle();
		public:
			// Destructor
			~RotateTo() = default;

			/**
			*	Creates RotateTo action
			*	@param duration Duration of action
			*	@param angle Angle to rotate
			*/
			static RotateTo* create(const float duration, const float angle);

			// override
			void setTarget(TransformNode* target);

			// ovrrides
			void update(const float delta) override;
		};
	}
}

#endif