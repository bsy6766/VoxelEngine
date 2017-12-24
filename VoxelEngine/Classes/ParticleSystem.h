#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

// voxel
#include <UIBase.h>

// glm
#include <glm\glm.hpp>

// cpp
#include <list>

namespace Voxel
{
	// Foward delcaration
	class DataTree;
	class Random;

	namespace UI
	{
		// Foward delcaration
		class Particle;

		/**
		*	@class ParticleSystem
		*	@brief Renders particle system in UI(2D) space.
		*
		*	Mode
		*	Gravity mode: Particles are affected by gravity and accelearations. 
		*	Radius mode: 
		*/
		class ParticleSystem : public RenderNode
		{
		public:
			enum class Mode
			{
				GRAVITY_MODE = 0,
				RADIUS_MODE
			};

			enum class PositionType
			{
				/**
				*	Particles are spawned in emitter's position but won't be affected by its translation.
				*	Use case: Smoke particle on moving car. Smoke spawnas at the car and stays even if the car keeps moving.
				*/
				PT_RELATIVE,
				/**
				*	All particles are affected by emitter's translation.
				*/
				PT_GROUPED,
			};
		private:
			// Types

			// Particle system mode
			Mode mode;

			// Position type
			PositionType positionType;

			// list of particles
			std::list<Particle*> particles;

			// number of living particles
			unsigned int livingParticleNumber;

			// Gravity mode attributes ----------

			// position 
			glm::vec2 emitPos;

			// position variance
			glm::vec2 emitPosVar;

			// gravity
			glm::vec2 gravity;
			
			// speed
			float speed;

			// speed variance
			float speedVar;

			// Radial acceleration
			float accelRad;

			// Radial acceleration variance
			float accelRadVar;

			// Tangent acceleration
			float accelTan;

			// Tangent acceleration
			float accelTanVar;

			// ----------------------------------

			// Radius mode attributes -----------
			// ----------------------------------

			// Shared atrributes ----------------

			// Size of particle
			unsigned int particleSize;
			
			// emit angle
			float emitAngle;

			// emit angle variance
			float emitAngleVar;

			// emission rate = (particleSize / particleLifeTime)
			float emissionRate;

			// particle life time
			float particleLifeSpan;

			// particle life time variance
			float particleLifeSpanVar;

			// duration of particle system. -1 means infinite
			float duration;

			// Elapsed time
			float elapsedTime;

			// Spawn point.
			float spawnPoint;

			// start color
			glm::vec4 startColor;

			// start color variance
			glm::vec4 startColorVar;

			// end color
			glm::vec4 endColor;

			// end color variance
			glm::vec4 endColorVar;

			// start size
			float startSize;

			// start size variance
			float startSizeVar;

			// end size
			float endSize;

			// end size variance
			float endSizeVar;

			// blend src
			unsigned int blendSrc;

			// blend dest
			unsigned int blendDest;

			// ----------------------------------
		private:
			// Constructor
			ParticleSystem() = delete;
			ParticleSystem(const std::string& name);

			// random generator
			Random* rand;

			// initialize
			bool init(Voxel::DataTree* particleSystemDataTree);
		public:
			// Destructor
			~ParticleSystem();

			// Create particle system with data tree file
			static ParticleSystem* create(const std::string& name, const std::string& dataFileName);

			// Set value from data tree
			bool setAttributesWithFile(Voxel::DataTree* particleSystemDataTree);

			// override update
			void update(const float delta) override;

			// render
			void renderSelf() override;
		};
	}
}

#endif