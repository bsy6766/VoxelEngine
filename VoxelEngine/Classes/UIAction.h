#ifndef UI_ACTION_H
#define UI_ACTION_H

namespace Voxel
{
	namespace UI
	{
		// Forward declaration
		class Sequence;
		class Action;
		class ActionPack;
		class MoveTo;
		class RotateTo;
		class ScaleTo;
		class FadeTo;
		class Delay;
		class ProgressTo;

		/**
		*	@class Sequence
		*	Sequence executes UIActions in order. Have few option to pause, repeat, etc
		*/
		class Sequence
		{
		private:
			Sequence();
		public:
			~Sequence();
		};
	}
}

#endif