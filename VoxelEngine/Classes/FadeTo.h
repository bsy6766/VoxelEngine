#ifndef FADE_TO_H
#define FADE_TO_H

// Voxel
#include "Action.h"

namespace Voxel
{
	namespace UI
	{
		// forward declaration
		class TransformNode;
		
		/**
		*	@class FadeTo
		*	@brief Change ui's opacity
		*/
		class FadeTo : public Action
		{
		private:
			// Constructor
			FadeTo();

			// opacity
			float opacity;

			// target. Need TransformNode at least.
			TransformNode* target;

			// init
			bool init(const float duration, const float opacity);
		public:
			// Destructor
			~FadeTo() = default;

			/**
			*	Creates FadeTo action
			*	@param duration Duration of action
			*	@param opacity Opacity to change
			*/
			static FadeTo* create(const float duration, const float opacity);

			// override
			void setTarget(TransformNode* target);

			// overrides
			void update(const float delta) override;
		};
	}
}

#endif