#ifndef SKYBOX_H
#define SKYBOX_H

#include <GL\glew.h>
#include <glm\glm.hpp>

namespace Voxel
{
	class Program;

	class Skybox
	{
	private:
		enum class FOG_STATE
		{
			IDLE = 0,
			ANIMATING,
		};

		enum class SKYCOLOR_STATE
		{
			IDLE = 0,
			ANIMATING,
		};

		FOG_STATE fogState;
		SKYCOLOR_STATE skycolorState;
	private:
		// True if fog is enabled
		bool fogEnabled;

		// Pointer to skybox shader program
		Program* skyboxProgram;

		// skybox's MVP matrix
		glm::mat4 MVP_Matrix;

		// Size of sky box
		float size;

		// OpenGL
		GLuint vao;
		int indicesSize;

		// sun (textured quad)
		GLuint sunVao;
		glm::vec3 sunColor;

		// Color blend between top color and bottom color
		glm::vec3 midBlend;

		// Distance of fog
		float fogDistance;
		float fogLength;
		
		// current distance of fog. For Fog animation
		float curFogDistance;

		// Speed of fog animation
		float fogAnimationSpeed;

		void initSkybox();
		void initSun();
		void initMoonPhases();
	public:
		// Constructor
		Skybox();

		// Destructor
		~Skybox();

		/**
		*	Initialize skybox.
		*	Set fog values and creates box vertices.
		*	@param [in] renderDistance Render distance of game.
		*/
		void init(const int renderDistance);

		/**
		*	Updates skybox. For animation.
		*	@param [in] delta Elapsed time for current frame.
		*/
		void update(const float delta);

		/**
		*	Updates skybox color based on time. updates mixBlend color.
		*	@param [in] hour Current hour in the game in 24 hours scale.
		*	@param [in] minute Current minute in the game. 0 ~ 59.
		*	@param [in] seconds Current seconds in the game. 0 ~ 1.0f
		*/
		void updateColor(const int hour, const int minute, const float seconds);

		/**
		*	Updates MVP matrix.
		*	@param [in] mat Projection * player's view * player's position without y.
		*/
		void updateMatrix(const glm::mat4& mat);

		/**
		*	Renders skybox
		*/
		void render();

		// Get fog distance
		float getFogDistance();

		float getFogLength();

		void setFogLength(const int chunkSize);

		/**
		*	Set fog distance by render distance.
		*	@param [in] renderDistance Render distance of game.
		*	@param [in] animate True to animate fog changes.
		*/
		void setFogDistanceByRenderDistance(const int renderDistance, const bool animate);

		/**
		*	Set fog distance
		*	@param [in] distance Fog distance to set.
		*	@param [in] animate True to animate fog changes.
		*/
		void setFogDistance(const float distance, const bool animate);

		// Check if fog is enabled
		bool isFogEnabled();

		// Set fog enabled.
		void setFogEnabled(const bool enabled);

		// Get mixblend color
		glm::vec3 getMidBlendColor();

		// Get ambient color based on time
		float getAmbientColor(const int hour, const int minute, const float second);
	};
}

#endif