#ifndef REPEAT_H
#define REPEAT_H

// voxel
#include "Action.h"

namespace Voxel
{
	class TransformNode;

	namespace UI
	{
		/**
		*	@class Repeat
		*	@brief This class is decorator to action that repeats action
		*/
		class Repeat : public Action
		{
		private:
			// constructor
			Repeat();

			// Action to decorate
			Action* action;

			// Number of repeat
			unsigned int repeat;
			unsigned int currentRepeat;

			// init
			bool init(Action* action, const unsigned int repeat);

		public:
			// Destructor
			~Repeat();

			// Create
			static Repeat* create(Action* action, const unsigned int repeat);

			// get repeat count
			unsigned int getRepeat() const;

			// Check if repeat is done
			bool isDone() const override;

			// reset
			void reset() override;

			// override
			float getExceededTime() override;

			// Set target to both actions
			void setTarget(TransformNode* target) override;

			// Update sequence
			void update(const float delta) override;
		};
	}
}

#endif