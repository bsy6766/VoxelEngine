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
			std::list<Particle*> deadParticles;

			// number of living particles
			unsigned int livingParticleNumber;

			// extra pool size
			const unsigned int ExtraParticlesInPool = 10;

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
			unsigned int totalParticles;
			
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

			// texture size (width only)
			float textureSize;

			// start size
			float startSize;

			// start size variance
			float startSizeVar;

			// end size
			float endSize;

			// end size variance
			float endSizeVar;

			// start angle
			float startAngle;

			// start angle variance
			float startAngleVar;

			// end angle
			float endAngle;

			// end angle variance
			float endAngleVar;

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

			// gl
			GLuint posbo;
			GLuint srbo;
			GLuint cbo;

			// initialize
			bool init(Voxel::DataTree* particleSystemDataTree);
		public:
			// Destructor
			~ParticleSystem();

			// Create particle system with data tree file
			static ParticleSystem* create(const std::string& name, const std::string& dataFileName);

			// Set value from data tree
			bool setAttributesWithFile(Voxel::DataTree* particleSystemDataTree);

			// set duration
			void setDuration(const float duration);

			// get emission rate
			float getEmissionRate() const;

			// get total living particles
			int getLivingParticleNumber() const;

			// set total particles
			void setTotalParticles(const unsigned int totalParticles);

			// set particle life span
			void setParticleLifeSpan(const float lifeSpan);

			// set particle life span var
			void setParticleLifeSpanVar(const float lifeSpanVar);

			// set speed
			void setSpeed(const float speed);

			// set speed var
			void setSpeedVar(const float speedVar);

			// set emit position
			void setEmitPositionX(const float x);
			void setEmitPositionY(const float y);
			void setEmitPosition(const glm::vec2& emitPos);

			// get emission position
			glm::vec2 getEmissionPosition() const;

			// set emit pos var y
			void setEmitPosXVar(const float xVar);

			// set emit pos var x
			void setEmitPosYVar(const float yVar);

			// get emit pos var
			glm::vec2 getEmissionPositionVar() const;

			// set gravity x
			void setGravityX(const float gravityX);

			// set gravity y
			void setGravityY(const float gravityY);

			// set emit angle
			void setEmitAngle(const float angle);

			// set emit angle var
			void setEmitAngleVar(const float angleVar);

			// set accel rad
			void setAccelRad(const float accelRad);

			// set accel rad var
			void setAccelRadVar(const float accelRadVar);

			// set accel tan
			void setAccelTan(const float accelTan);

			// set accel tan var
			void setAccelTanVar(const float accelTanVar);

			// set start size
			void setStartSize(const float size);

			// set start size var
			void setStartSizeVar(const float size);

			// set end size
			void setEndSize(const float size);

			// set end size var
			void setEndSizeVar(const float size);

			// set start angle
			void setStartAngle(const float angle);

			// set start angle var
			void setStartAngleVar(const float angle);

			// set end angle
			void setEndAngle(const float angle);

			// set end angle var
			void setEndAngleVar(const float angle);

			// set start color
			void setStartColor(const glm::vec4& color);

			// set start color var
			void setStartColorVar(const glm::vec4& color);

			// set end color
			void setEndColor(const glm::vec4& color);

			// set end color var
			void setEndColorVar(const glm::vec4& color);

			// override update
			void update(const float delta) override;

			// render
			void renderSelf() override;
		};
	}
}

#endif