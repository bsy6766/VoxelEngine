#ifndef WEATHER_SYSTEM_H
#define WEATHER_SYSTEM_H

namespace Voxel
{
	/**
	*	@class WeatherSystem
	*	@brief System that manages weather in the game.
	*
	*	Weather system manages everything that related to weather like rain, snow, thunder, clouds.
	*
	*	Clouds
	*	Clouds are treated as entity. It gets updated and floats in the sky. 
	*	Once cloud are completely out of sight, it repositions itself to opposite site.
	*	Clouds have global direction they flow.
	*/
	class WeatherSystem
	{
	public:
		WeatherSystem();
		~WeatherSystem();
	};
}

#endif