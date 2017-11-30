#ifndef	ANIMATED_IMAGE_H
#define ANIMATED_IMAGE_H

#include "UIBase.h"

namespace Voxel
{
	namespace UI
	{
		/**
		*	@class AnimatedImage
		*	@brief A series of image frame that animates with interval.
		*
		*	Similar to image but have multiple image frames that animates within given interval.
		*	Animated image is static and won't be able to modify once created.
		*	Yet, can change few attributes such as interval, repeat, etc.
		*
		*	Image frame format must be ImageFrameName_FrameNumber.png, where ImageFrameName is image file name and FrameNumber is frame number.
		*	For example, image file name 'player_anim" and 3 frames will be 'player_anim_0.png', 'player_anim_1.png" and 'player_anim_2.png'.
		*/
		class AnimatedImage : public RenderNode
		{
		private:
			AnimatedImage() = delete;

			// Constructor
			AnimatedImage(const std::string& name);

			// Frame size of animation. Must be positive number
			int frameSize;

			// Interval between frames.
			float interval;

			// Currently elapsed time to keep track interval.
			float elapsedTime;

			// Current frame index. Starts from 0.
			int currentFrameIndex;

			// Size of rames
			std::vector<glm::vec2> frameSizes;

			// gl
			unsigned int currentIndex;

			// Repeats animation if this is true. Else, stops on last frame.
			bool repeat;

			// If animation is stopped, can't be paused or resumed. Must call start() to run animation from the beginning.
			bool stopped;

			// If animation is paused, then it can either call start() to start over animation or call resume() to resume animation.
			bool paused;

			/**
			*	Initialize animated image
			*	@param ss SpriteSheet pointer
			*	@param frameName Image frame file name.
			*	@param frameSize Size of frame. Must be greater than 0.
			*	@param interval Interval between frame.
			*	@param repeat true if animation repeats. Else false.
			*/
			bool init(SpriteSheet* ss, const std::string& frameName, const int frameSize, const float interval, const bool repeat);

			/**
			*	Build image.
			*	Can't use array because frame size is dynamic
			*	@param vertices Vertices of image quad.
			*	@param uvs Texture coordinates of image
			*	@param indices Indices of image quad.
			*/
			virtual void build(const std::vector<float>& vertices, const std::vector<float>& uvs, const std::vector<unsigned int>& indices);
		public:
			// Desturctor
			~AnimatedImage() = default;

			/**
			*	Initialize animated image
			*	@param spriteSheetName Name of sprite sheet that has image frames. All frames must be in same sprite sheet.
			*	@param frameName Image frame file name.
			*	@param frameSize Size of frame. Must be greater than 0.
			*	@param interval Interval between frame. Must be greater than 0.
			*	@param repeat true if animation repeats. Else false.
			*/
			static AnimatedImage* create(const std::string& name, const std::string& spriteSheetName, const std::string& frameName, const int frameSize, const float interval, const bool repeat);

			/**
			*	Start animation from the first frame
			*/
			void start();

			/**
			*	Pause animation
			*/
			void pause();

			/**
			*	Resume animation
			*/
			void resume();

			/**
			*	Stop animation.
			*/
			void stop();

			/**
			*	Set interval. Must be greater than 0.
			*/
			void setInterval(const float interval);

			/**
			*	Override bases update function
			*/
			void update(const float delta) override;

			/**
			*	Render self
			*/
			void renderSelf() override;
		};
	}
}

#endif