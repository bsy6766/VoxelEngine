#ifndef REPEATFOREVER_H
#define REPEATFOREVER_H

// voxel
#include "Action.h"

namespace Voxel
{
	class TransformNode;

	namespace UI
	{
		/**
		*	@class RepeatForever
		*	@brief This class is decorator to action that repeats action forever
		*/
		class RepeatForever : public Action
		{
		private:
			// constructor
			RepeatForever();

			// Action to decorate
			Action* action;
			
			// init
			bool init(Action* action);

		public:
			// Destructor
			~RepeatForever();

			// Create
			static RepeatForever* create(Action* actiont);
			
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