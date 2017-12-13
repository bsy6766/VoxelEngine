#ifndef VISIBILITY_H
#define VISIBILITY_H

// Voxel
#include "Action.h"

namespace Voxel
{
	namespace UI
	{
		/**
		*	@class Visibility
		*	@brief UI Action class that changes visibility after duration
		*/
		class Visibility : public Action
		{
		private:
			// Constructor
			Visibility();

			// opacity
			bool visibility;

			// target. Need TransformNode at least.
			TransformNode* target;

			// init
			bool init(const float duration, const bool visibility);
		public:
			// Destructor
			~Visibility() = default;

			/**
			*	Creates Visibility action
			*	@param duration Duration of action
			*	@param visibility Visibility to set
			*/
			static Visibility* create(const float duration, const bool visibility);

			// override
			void setTarget(TransformNode* target);

			// overrides
			void update(const float delta) override;
		};
	}
}

#endif