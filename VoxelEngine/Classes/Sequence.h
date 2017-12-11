#ifndef UI_ACTION_H
#define UI_ACTION_H

// cpp
#include <vector>
#include <initializer_list>

// voxel
#include "Action.h"

namespace Voxel
{
	namespace UI
	{
		class TransformNode;

		/**
		*	@class Sequence
		*	@Brief A sequence of action
		*
		*	Sequence is sequence of actions that executes actions in order.
		*	Sequence with 1 action will still be sequence, but will act same as just action itself. There is no point of making sequence with 1 action.
		*	Sequence with 2 actions will first excute first action and then second action.
		*	Sequence with mutliple actions will converted to nested two actions sequence.
		*	For example, actions [1,2,3,4] will be [[1, 2], 3] and 4.
		*/
		class Sequence : public Action
		{
		private:
			// Constructor
			Sequence();

			// First action
			Action* first;
			// Second action
			Action* second;

			// Initialize with 1 action
			bool init(Action* action);

			// Initialize with 2 actions
			bool init(Action* first, Action* second);

			// Initialize with multiple actions
			bool init(const std::initializer_list<Action*>& actions);
		public:
			// Destructor
			~Sequence();

			// Create with 1 action
			static Sequence* create(Action* action);

			// Create with 2 actions
			static Sequence* create(Action* first, Action* second);

			// Create with multiple actions
			static Sequence* create(const std::initializer_list<Action*>& actions);

			// Check if sequence is done
			bool isDone() const override;

			// Set target to both actions
			void setTarget(TransformNode* target) override;

			// Update sequence
			void update(const float delta) override;
		};
	}
}

#endif