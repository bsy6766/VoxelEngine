#ifndef TINT_TO_H
#define TINT_TO_H

// voxel
#include "Action.h"

namespace Voxel
{
	namespace UI
	{
		// forward declaration
		class RenderNode;
		class TransformNode;

		/**
		*	@class RotateTo
		*	@brief Rotate ui to specific angle
		*/
		class TintTo : public Action
		{
		private:
			// Constructor
			TintTo();

			// target. Need TransformNode at least.
			RenderNode* target;

			// color
			glm::vec3 color;

			// init
			bool init(const float duration, const glm::vec3& color);
		public:
			// Destructor
			~TintTo() = default;

			/**
			*	Creates RotateTo action
			*	@param duration Duration of action
			*	@param angle Angle to rotate
			*/
			static TintTo* create(const float duration, const glm::vec3& color);

			// override
			void setTarget(TransformNode* target);

			// ovrrides
			void update(const float delta) override;
		};
	}
}

#endif