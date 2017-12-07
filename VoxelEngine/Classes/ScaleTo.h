#ifndef SCALE_TO_H
#define SCALE_TO_H

// Voxel
#include "Action.h"

namespace Voxel
{
	namespace UI
	{
		// forward declaration
		class TransformNode;

		/**
		*	@class ScaleTo
		*	@brief Scale ui to specific scale
		*/
		class ScaleTo : public Action
		{
		private:
			// Constructor
			ScaleTo();

			// scale in x and y axis
			glm::vec2 scale;

			// target. Need TransformNode at least.
			TransformNode* target;

			// init
			bool init(const float duration, const glm::vec2& scale);

		public:
			// Destructor
			~ScaleTo() = default;

			/**
			*	Creates RotateTo action
			*	@param duration Duration of action
			*	@param scale Scale in x y axis.
			*/
			static ScaleTo* create(const float duration, const glm::vec2& scale);

			// override
			void setTarget(TransformNode* target);

			// ovrrides
			void update(const float delta) override;
		};
	}
}

#endif