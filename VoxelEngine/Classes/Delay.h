#ifndef DELAY_H
#define DELAY_H

// Voxel
#include "Action.h"

namespace Voxel
{
	namespace UI
	{
		/**
		*	@class Delay
		*	@brief Does nothing.
		*/
		class Delay : public Action
		{
		private:
			// Default constructor
			Delay();

			// initialize
			bool init(const float duration);
		public:
			// Default destructor
			~Delay() = default;

			/**
			*	Create delay action
			*	@param duration Duration of delay
			*/
			static Delay* create(const float duration);

			// ovrrides
			void update(const float delta) override;
		};
	}
}

#endif