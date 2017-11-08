#ifndef CALENDAR_H
#define CALENDAR_H

#include <string>
#include <functional>

namespace Voxel
{
	/**
	*	@class Calendar
	*	@brief Manages date and time in game world
	*
	*	World follows the same day system in real life. 
	*	A week is 7 days (Monday to Sunday) and each day
	*	has unique buffs starts from 0:00 to 23:59
	*
	*	Instead of month like in real life, the world has
	*	season.
	*
	*	Event days
	*/
	class Calendar
	{
	public:
		enum class DAY : int
		{
			MONDAY = 0,
			TUESDAY,
			WEDNESDAY,
			THURSDAY,
			FRIDAY,
			SATURDAY,
			SUNDAY
		};
	private:
		const static int DAYS_PER_WEEK;
		// amount of tick(IRL time) equals to minute in world
		const static float MIN_TICK;
	private:
		// Date
		int day;
		DAY curDAY;
		
		// Time
		int minute;
		int hour;

		// Game tick measurement
		float second;

		DAY getNextDAY();
	public:
		Calendar();
		~Calendar();

		// Initialialize time to morning 10 am and first day of world.
		void init();
		// Initialize time with specific time
		void init(const int minute, const int hour);

		// update time
		void update(const float delta);

		// Get time in string
		std::string getTimeInStr(const bool twelveHour);

		// Get time in number. HHMM. i.e.) 1000 = 10am, 2330 = 11:30pm
		int getTime();

		// Get current housr
		int getHour();

		// Get current minutes
		int getMinutes();

		// get seconds
		float getSeconds();

		// Set time
		void setTime(const int hour, const int minute);

		// add time
		void addTime(const int hour, const int minute);
	};

}
#endif