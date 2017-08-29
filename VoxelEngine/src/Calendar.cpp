#include <Calendar.h>

using namespace Voxel;

const int Calendar::DAYS_PER_WEEK = 7;
const float Calendar::MIN_TICK = 2.0f;

Calendar::Calendar()
	: curDAY(DAY::MONDAY)
	, minute(0)
	, hour(0)
	, elapsedTime(0)
{

}

Calendar::~Calendar()
{

}

void Voxel::Calendar::init()
{
	curDAY = DAY::MONDAY;
	minute = 0;
	hour = 9;
}

void Voxel::Calendar::update(const float delta)
{
	elapsedTime += delta;
	
	if (elapsedTime > Calendar::MIN_TICK)
	{
		while (elapsedTime > Calendar::MIN_TICK)
		{
			minute++;
			elapsedTime -= Calendar::MIN_TICK;

			if (minute >= 60)
			{
				hour++;
				minute = 0;

				if (hour >= 24)
				{
					day++;
					curDAY = getNextDAY();
					hour = 0;
				}
			}
		}
	}
}

std::string Voxel::Calendar::getTimeInStr(const bool twelveHour)
{
	std::string time = ":";

	if (minute < 10)
	{
		time = time + "0" + std::to_string(minute);
	}
	else
	{
		time += std::to_string(minute);
	}

	if (twelveHour)
	{
		std::string hourStr;
		if (hour > 12)
		{
			hourStr = std::to_string(hour - 12);
		}
		else
		{
			hourStr = std::to_string(hour);
		}

		time = hourStr + time;
		std::string AmPm = (hour >= 12) ? "PM" : "AM";

		time += AmPm;

		return time;
	}
	else
	{
		return std::to_string(hour) + time;
	}
}

Calendar::DAY Voxel::Calendar::getNextDAY()
{
	switch (curDAY)
	{
	case Voxel::Calendar::DAY::MONDAY:
		return Calendar::DAY::TUESDAY;
		break;
	case Voxel::Calendar::DAY::TUESDAY:
		return Calendar::DAY::WEDNESDAY;
		break;
	case Voxel::Calendar::DAY::WEDNESDAY:
		return Calendar::DAY::THURSDAY;
		break;
	case Voxel::Calendar::DAY::THURSDAY:
		return Calendar::DAY::FRIDAY;
		break;
	case Voxel::Calendar::DAY::FRIDAY:
		return Calendar::DAY::SATURDAY;
		break;
	case Voxel::Calendar::DAY::SATURDAY:
		return Calendar::DAY::SUNDAY;
		break;
	case Voxel::Calendar::DAY::SUNDAY:
		return Calendar::DAY::MONDAY;
		break;
	default:
		break;
	}

	return Calendar::DAY::MONDAY;
}