#ifndef PROGRESS_TIMER_H
#define PROGRESS_TIMER_H

#include "UIBase.h"

namespace Voxel
{
	namespace UI
	{
		/**
		*	@class ProgressTimer
		*	@brief A simple progress timer. Can be BAR type or RADIAL type.
		*/
		class ProgressTimer : public RenderNode
		{
		public:
			enum class Type
			{
				HORIZONTAL = 0,
				VERTICAL,
				RADIAL
			};

			enum class Direction
			{
				CLOCK_WISE = 0,
				COUNTER_CLOCK_WISE
			};
		private:
			// Constructor
			ProgressTimer() = delete;
			ProgressTimer(const std::string& name);

			// percenatge. 0 ~ 100
			int percentage;

			// current index
			int currentIndex;

			// type of progress bar
			Type type;

			/**
			*	Initialize button
			*/
			bool init(SpriteSheet* ss, const std::string& progressTimerImageFileName, const Type type = Type::HORIZONTAL, const Direction direction = Direction::CLOCK_WISE);

			void buildMesh(const glm::vec2& verticesOrigin, const glm::vec2& verticesEnd, const glm::vec2& uvOrigin, const glm::vec2& uvEnd, std::vector<float>& vertices, std::vector<float>& uvs, std::vector<unsigned int>& indices, const Direction direction);

			/**
			*	Build image.
			*	Initialize vao.
			*	@param vertices Vertices of image quad
			*	@param uvs Texture coordinates of image quad
			*	@param indices Indices of image quad
			*/
			void build(const std::vector<float>& vertices, const std::vector<float>& uvs, const std::vector<unsigned int>& indices);

			/**
			*	Updates current index based on progress timer.
			*	Background, percentage and type affects index.
			*/
			void updateCurrentIndex();
		public:
			// Desturctor
			~ProgressTimer() = default;

			/**
			*	Create progress timer
			*/
			static ProgressTimer* create(const std::string& name, const std::string& spriteSheetName, const std::string& progressTimerImageFileName, const Type type = Type::HORIZONTAL, const Direction direction = Direction::CLOCK_WISE);

			/**
			*	Set percentage.
			*	@param precentage. Must be 0 ~ 100
			*/
			void setPercentage(const int percentage);

			/**
			*	Get percentage
			*/
			int getPercentage() const;

			/**
			*	Render self
			*/
			void renderSelf() override;
		};
	}
}

#endif
