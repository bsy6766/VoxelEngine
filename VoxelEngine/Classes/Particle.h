#ifndef PARTICLE_H
#define PARTICLE_H

// glm
#include <glm\glm.hpp>

namespace Voxel
{
	namespace UI
	{
		/**
		*	@class Particle
		*	@brief Contains data for each particle
		*/
		class Particle
		{
			// Only particle system can access to Particle
			friend class ParticleSystem;
		private:
			// Constructor
			Particle();

			// id
			int id;

			// id counter
			static int idCounter;

			// alive state
			bool alive;

			// current position
			glm::vec2 pos;

			// Color
			glm::vec4 startColor;
			glm::vec4 endColor;

			// speed
			float speed;

			// dir vec
			glm::vec2 dirVec;

			// accel
			float accelRad;
			float accelTan;

			// life span
			float lifeSpan;
			float livedTime;

			// size (scale)
			float startSize;
			float endSize;

			// angle (rotation)
			float startAngle;
			float endAngle;

			void normalizePoint(glm::vec2& radial);
		public:
			~Particle() = default;
		};
	}
}

#endif