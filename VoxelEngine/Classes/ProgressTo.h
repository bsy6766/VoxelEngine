#ifndef PROGRESS_TO_H
#define PROGRESS_TO_H

// Voxel
#include "Action.h"

namespace Voxel
{
	namespace UI
	{
		// forward declaration
		class ProgressTimer;

		/**
		*	@class ProgressTo
		*	@brief Progress ProgressTimer to specific percentage.
		*/
		class ProgressTo : public Action
		{
		private:
			// Constructor
			ProgressTo();

			// percentage
			float percentage;

			// target. Must be progress timer
			ProgressTimer* target;

			// init
			bool init(const float duration, const float percentage);

		public:
			// Destructor
			~ProgressTo() = default;

			/**
			*	Creates FadeTo action
			*	@param duration Duration of action
			*	@param opacity Opacity to change
			*/
			static ProgressTo* create(const float duration, const float percentage);

			// overrides
			void update(const float delta) override;

			// override
			void setTarget(TransformNode* progressTimer) override;
		};
	}
}

#endif