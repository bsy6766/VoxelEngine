#ifndef UI_ACTION_H
#define UI_ACTION_H

// cpp
#include <vector>
#include <initializer_list>

namespace Voxel
{
	namespace UI
	{
		class Action;
		class TransformNode;

		/**
		*	@class Sequence
		*	Sequence executes single or multiple actions in order.
		*/
		class Sequence
		{
		public:
			enum class State
			{
				RUNNING = 0,
				PAUSED,
				STOPPED,
				FINISHED,
			};
		private:
			// Constructor
			Sequence();

			// actions
			std::vector<Action*> actions;

			// repeat
			bool repeating;

			// State
			State sequenceState;

			// Current action index
			int currenActionIndex;

			// initailize sequence
			bool init(Action* action, const bool repeat);
			bool init(const std::initializer_list<Action*>& actions, const bool repeat);
		public:
			// Destructor
			~Sequence();

			/**
			*	Create sequence with single action
			*	@param action An action to execute
			*	@param repeat true if action needs to repeat. Else, false.
			*/
			static Sequence* create(Action* action, const bool repeat);

			/**
			*	Create sequence with multiple action
			*	@param actions List of actions to execute
			*	@param repeat true if action needs to repeat. Else, false.
			*/
			static Sequence* create(const std::initializer_list<Action*>& actions, const bool repeat);

			// repeat sequence
			void repeat();

			// Check if sequence repeats
			bool isRepeating() const;

			// Stop sequence
			void stop();

			// Check if sequence is finished
			bool isFinished();

			// Start sequence from the beginning
			void start();

			// Pause sequence
			void pause();

			// resume sequence
			void resume();

			// Update actions
			void update(const float delta);

			// Set target of sequence
			void setTarget(TransformNode* target);
		};
	}
}

#endif