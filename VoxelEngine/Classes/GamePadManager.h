#ifndef JOYSTICK_MANAGER_H
#define JOYSTICK_MANAGER_H

// cpp
#include <unordered_map>
#include <functional>

// sdl2
#include <SDL2/SDL.h>

#define MAX_JOYSTICK 4

namespace Voxel
{
	class GamePadManager;

	typedef Sint16 GamePadID;

	namespace GAMEPAD
	{
		namespace XBOX_360
		{
			// Mapped to SDL2's controller button number
			enum class BUTTON
			{
				A = 0,			// 0
				B,				// 1
				X,				// 2
				Y,				// 3
				BACK,			// 4
				NONE,			// 5
				START,			// 6
				L3,				// 7
				R3,				// 8
				LS,				// 9
				RS,				// 10
				DPAD_UP,		// 11
				DPAD_DOWN,		// 12
				DPAD_LEFT,		// 13
				DPAD_RIGHT		// 14
			};

			enum class AXIS
			{
				L_AXIS_X = 0,
				L_AXIS_Y,
				R_AXIS_X,
				R_AXIS_Y,
				LT,
				RT,
			};
		}

		/*
		namespace XBOX_ONE
		{

		}
		*/
	}

	class GamePad
	{
	private:
		// Manager class if friend
		friend GamePadManager;

		// Private constructor. User can't make their own controller instance.
		GamePad(SDL_GameController* controller, SDL_Haptic* haptic, const std::string& name, const GamePadID id);

		// Private destructor. Only manager can delete instance.
		~GamePad();

		// SDL game controller
		SDL_GameController* controller;

		// SDL haptic (optional)
		SDL_Haptic* haptic;

		// name of controller
		std::string name;

		// id
		GamePadID id;

		// Note: SDL supports balls for controller, but we will focus on xbox360 for windows.
		bool hapticEnabled;

		// Haptic modifier. 1.0 by default(100%)
		float hapticModifier;

		// Maximum and minimum value that controller can make
		const float SDL_AXIS_MAX_ABS_VALUE = 32767;
		const float SDL_AXIS_MIN_ABS_VALUE = 32768;

		/**
		*	Most controller's axis aren't 'exactly' precise.
		*	Eventhough axises are idle, they are slightly tilted not centered prefectly which means that
		*	axis value aren't always 0 at idle. (The reason can be old controller or losen axises, etc)
		*	Therefore, we set a minimum and maxium value and determines whether if player really made an
		*	input physically or not by comparing input value with min/max setting.
		*
		*	By default, it's set to -10000, 10000 each
		*/
		Sint16 AXIS_MIN;
		Sint16 AXIS_MAX;

		// Button state
		std::unordered_map<GAMEPAD::XBOX_360::BUTTON, bool> buttonStateMap;
		std::unordered_map<GAMEPAD::XBOX_360::BUTTON, bool> buttonStateTickMap;

		// Axis movement state
		std::unordered_map<GAMEPAD::XBOX_360::AXIS, float> axisValueMap;

		/**
		*	Update button state
		*/
		void updateButtonState(GAMEPAD::XBOX_360::BUTTON button, bool state);

		/**
		*	Update axis value
		*/
		void updateAxisValue(GAMEPAD::XBOX_360::AXIS axis, float value);

		/**
		*	Gets axis value based on each controller setting.
		*	SDL returns -32768 ~ 32767 value on axis movement.
		*	We convert axis value in 0.0f ~ 1.0f scale. 
		*/
		const float convertAxisValue(GamePadID rawValue, const float modifier = 1.0f);

		// Play rumble effect
		void playRumble(const float strength, const Uint32 length);
		
		// Check if button was pressed
		bool isButtonPressed(GAMEPAD::XBOX_360::BUTTON button, const bool tick = false);

		// Check if button was released
		bool isButtonReleased(GAMEPAD::XBOX_360::BUTTON button, const bool tick = false);

		// Get axis value
		float getAxisValue(GAMEPAD::XBOX_360::AXIS axis);

		// post update. Wipes button tick states
		void postUpdate();
	public:
	};

	/**
	*	@class GamePadManager
	*
	*	@brief Manages multiple controllers connections.
	*
	*	@note Only supports XBOX controllers
	*/
	class GamePadManager
	{
	private:
		// Constructor
		GamePadManager();

		// Destructor
		~GamePadManager();

		//Singleton instance
		static GamePadManager* instance;

		// True if sdl is usable.
		bool active;

		// Store controller
		std::unordered_map<GamePadID/*SDL controller id*/, GamePad*> gamePads;

		/**
		*	Add controller to manager
		*/
		void addController(const SDL_ControllerDeviceEvent event);

		/**
		*	Remove controller from manager
		*/
		void removeController(const SDL_ControllerDeviceEvent event);

		/**
		*	Button pressed
		*	This function is called when SDL detects button press.
		*	It stores down state for each button pressed.
		*	Also it calls onButtonPressed if has.
		*/
		void buttonPressed(GamePadID id, const SDL_ControllerButtonEvent event);

		/**
		*	Button released
		*	Same mechanism with buttonPressed function but just for released/up state
		*/
		void buttonReleased(GamePadID id, const SDL_ControllerButtonEvent event);

		/**
		*	Axis moved
		*	This function is called when SDL detects axis movement.
		*/
		void axisMoved(GamePadID id, const SDL_ControllerAxisEvent event);

		/**
		*	Finds controller by id.
		*	Nullptr if doesn't exists
		*/
		GamePad* findGamePad(GamePadID id) const;

	public:
		// Get instance
		static GamePadManager* getInstance();

		// Destroy instance.
		static void deleteInstance();

		// Prevent copying or assigning instance
		GamePadManager(GamePadManager const&) = delete;
		void operator=(GamePadManager const&) = delete;

		const float SDL_AXIS_MAX_ABS_VALUE = 32767;
		const float SDL_AXIS_MIN_ABS_VALUE = 32768;

		/**
		*	Update manager.
		*	@note Call this whenever you want to update input. Recommended to call every frame or tick.
		*/
		void update();

		// Callback function when button is pressed
		static std::function<void(GamePadID id, GAMEPAD::XBOX_360::BUTTON button)> onButtonPressed;

		// Callback function when button is released.
		static std::function<void(GamePadID id, GAMEPAD::XBOX_360::BUTTON button)> onButtonReleased;

		// Callback function when axis moved.
		static std::function<void(GamePadID id, GAMEPAD::XBOX_360::AXIS axis, const float value)> onAxisMoved;

		// Callback function when controller is connected
		static std::function<void(GamePadID id)> onControllerConnected;

		// Callback function when controller is disconnected
		static std::function<void(GamePadID id)> onControllerDisconnected;

		// Get/Set minimum axis value
		Sint16 getMinAxisValue(GamePadID id) const;
		void setMinAxisValue(GamePadID id, Sint16 value);

		// Get/Set maximum axis value
		Sint16 getMaxAxisValue(GamePadID id) const;
		void setMaxAxisValue(GamePadID id, Sint16 value);

		// Haptic modifier (Vibration power)
		float getHapticModifier(GamePadID id) const;
		void setHapticModifier(GamePadID id, float modifier);

		/**
		*	Check if button was pressed
		*	@param id GamePad id to check
		*	@param button Button to check
		*	@param tick true to check if button was pressed current frame. Else, false.
		*/
		bool isButtonPressed(GamePadID id, GAMEPAD::XBOX_360::BUTTON button, const bool tick = false) const;

		/**
		*	Check if button was released
		*	@param id GamePad id to check
		*	@param button Button to check
		*	@param tick true to check if button was released current frame. Else, false.
		*/
		bool isButtonReleased(GamePadID id, GAMEPAD::XBOX_360::BUTTON button, const bool tick = false) const;

		/**
		*	Check if axis moved 
		*	@param id GamePad id to check
		*	@param axis Axis to check
		*/
		bool isAxisMoved(GamePadID id, GAMEPAD::XBOX_360::AXIS axis) const;

		// Check if has haptic
		bool hasHaptic(GamePadID id) const;

		// Play rumble.
		void playRumble(GamePadID id, float strength, Uint32 length);

		// Get axis value for controller. returns 0 if controller is not found
		float getAxisValue(GamePadID id, GAMEPAD::XBOX_360::AXIS axis);

		// Get number of controllers connected
		int getControllerCount();

		// Check if it has at least one active controller
		bool hasController();
	};
}

#endif