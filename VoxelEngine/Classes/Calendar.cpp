#include "Calendar.h"

using namespace Voxel;

const int Calendar::DAYS_PER_WEEK = 7;
const float Calendar::MIN_TICK = 1.0f;

Calendar::Calendar()
	: curDAY(DAY::MONDAY)
	, minute(0)
	, hour(0)
	, second(0)
{

}

Calendar::~Calendar()
{

}

void Voxel::Calendar::init()
{
	curDAY = DAY::MONDAY;
	minute = 0;
	hour = 7;
}

void Voxel::Calendar::update(const float delta)
{
	second += delta;
	
	if (second > Calendar::MIN_TICK)
	{
		while (second > Calendar::MIN_TICK)
		{
			minute++;
			second -= Calendar::MIN_TICK;

			while (minute >= 60)
			{
				hour++;
				minute -= 60;

				while (hour >= 24)
				{
					day++;
					curDAY = getNextDAY();
					hour -= 24;
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

int Voxel::Calendar::getTime()
{
	return (hour * 100) + minute;
}

int Voxel::Calendar::getHour()
{
	return hour;
}

int Voxel::Calendar::getMinutes()
{
	return minute;
}

float Voxel::Calendar::getSeconds()
{
	return second;
}

void Voxel::Calendar::setTime(const int hour, const int minute)
{
	this->hour = hour;
	this->minute = minute;
	this->second = 0.0f;

	if (this->hour >= 24)
	{
		this->hour = this->hour % 24;
	}

	if (this->minute >= 60)
	{
		this->minute = this->minute % 60;
	}
}

void Voxel::Calendar::addTime(const int hour, const int minute)
{
	this->hour += hour;
	this->minute += minute;

	while (this->minute >= 60)
	{
		this->hour++;
		this->minute -= 60;
	}

	while (this->hour >= 24)
	{
		this->day++;
		curDAY = getNextDAY();

		this->hour -= 24;
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