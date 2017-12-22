#ifndef PROGRESS_TIMER_H
#define PROGRESS_TIMER_H

// voxel
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

			enum class State
			{
				IDLE = 0,
				HOVERED,
				CLICKED,
			};
		private:
			// Constructor
			ProgressTimer() = delete;
			ProgressTimer(const std::string& name);

			// percenatge. 
			float percentage;

			// current index
			int currentIndex;

			// type of progress bar
			Type type;
			
			// state
			State state;
			
			// Initialize progress timer
			bool init(const std::string& spriteSheetName, const std::string& progressTimerImageFileName, const Type type = Type::HORIZONTAL, const Direction direction = Direction::CLOCK_WISE);

			// build mesh
			void buildBuffers(const glm::vec2& verticesOrigin, const glm::vec2& verticesEnd, const glm::vec2& uvOrigin, const glm::vec2& uvEnd, std::vector<float>& vertices, std::vector<float>& uvs, std::vector<unsigned int>& indices, const Direction direction);

			// build buffer base on type and direction
			void loadBuffers(const std::vector<float>& vertices, const std::vector<float>& uvs, const std::vector<unsigned int>& indices);

			// Update vertex index based on percetnage
			void updateCurrentIndex();

			// update mouse move
			bool updateProgressTimerMouseMove(const glm::vec2& mousePosition, const glm::vec2& mouseDelta);
		public:
			// Desturctor
			~ProgressTimer() = default;

			// create progress timer
			static ProgressTimer* create(const std::string& name, const std::string& spriteSheetName, const std::string& progressTimerImageFileName, const Type type = Type::HORIZONTAL, const Direction direction = Direction::CLOCK_WISE);

			/**
			*	Set percentage.
			*	@param precentage Percentage to set. Value less than 0 or greater than 100.0f gets clampped to [0, 100].
			*/
			void setPercentage(const float percentage);

			// get percentage
			float getPercentage() const;

			// Mouse event overrides
			void updateMouseMoveFalse() override;
			bool updateMouseMove(const glm::vec2& mousePosition, const glm::vec2& mouseDelta) override;
			bool updateMousePress(const glm::vec2& mousePosition, const int button) override;
			bool updateMouseRelease(const glm::vec2& mousePosition, const int button) override;

			// render
			void renderSelf() override;
		};
	}
}

#endif
