#include "GamePadManager.h"

// cpp
#include <iostream>
#include <string>

// voxel
#include "Logger.h"
#include "ErrorCode.h"
#include "Config.h"

using namespace std;
using namespace Voxel;

GamePadManager* GamePadManager::instance = nullptr;

std::function<void(GamePadID, GAMEPAD::XBOX_360::BUTTON)> GamePadManager::onButtonPressed = nullptr;
std::function<void(GamePadID, GAMEPAD::XBOX_360::BUTTON)> GamePadManager::onButtonReleased = nullptr;
std::function<void(GamePadID, GAMEPAD::XBOX_360::AXIS, const float)> GamePadManager::onAxisMoved = nullptr;
std::function<void(GamePadID)> GamePadManager::onControllerConnected = nullptr;
std::function<void(GamePadID)> GamePadManager::onControllerDisconnected = nullptr;

GamePad::GamePad(SDL_GameController* controller, SDL_Haptic* haptic, const std::string& name, const GamePadID id) 
	: controller(controller),
	haptic(haptic),
	name(name), 
	id(id), 
	hapticEnabled(false),
	hapticModifier(1.0f),
	AXIS_MAX(10000), 
	AXIS_MIN(-10000)
{
	// Reset axis value to 0
	this->axisValueMap[GAMEPAD::XBOX_360::AXIS::L_AXIS_X] = 0;
	this->axisValueMap[GAMEPAD::XBOX_360::AXIS::L_AXIS_Y] = 0;
	this->axisValueMap[GAMEPAD::XBOX_360::AXIS::R_AXIS_X] = 0;
	this->axisValueMap[GAMEPAD::XBOX_360::AXIS::R_AXIS_Y] = 0;
	this->axisValueMap[GAMEPAD::XBOX_360::AXIS::LT] = 0;
	this->axisValueMap[GAMEPAD::XBOX_360::AXIS::RT] = 0;

	// Reset button state to false
	this->buttonStateMap[GAMEPAD::XBOX_360::BUTTON::A] = false;
	this->buttonStateMap[GAMEPAD::XBOX_360::BUTTON::B] = false;
	this->buttonStateMap[GAMEPAD::XBOX_360::BUTTON::X] = false;
	this->buttonStateMap[GAMEPAD::XBOX_360::BUTTON::Y] = false;
	this->buttonStateMap[GAMEPAD::XBOX_360::BUTTON::BACK] = false;
	this->buttonStateMap[GAMEPAD::XBOX_360::BUTTON::START] = false;
	this->buttonStateMap[GAMEPAD::XBOX_360::BUTTON::L3] = false;
	this->buttonStateMap[GAMEPAD::XBOX_360::BUTTON::R3] = false;
	this->buttonStateMap[GAMEPAD::XBOX_360::BUTTON::LS] = false;
	this->buttonStateMap[GAMEPAD::XBOX_360::BUTTON::RS] = false;
	this->buttonStateMap[GAMEPAD::XBOX_360::BUTTON::DPAD_UP] = false;
	this->buttonStateMap[GAMEPAD::XBOX_360::BUTTON::DPAD_DOWN] = false;
	this->buttonStateMap[GAMEPAD::XBOX_360::BUTTON::DPAD_LEFT] = false;
	this->buttonStateMap[GAMEPAD::XBOX_360::BUTTON::DPAD_RIGHT] = false;

	// Check if can rumble
	if (SDL_HapticRumbleSupported(haptic))
	{
		if (SDL_HapticRumbleInit(haptic))
		{
			hapticEnabled = true;
		}
		else
		{
			hapticEnabled = false;
		}
	}
}

GamePad::~GamePad()
{
	if (this->haptic != nullptr)
	{
		SDL_HapticClose(this->haptic);
	}

	if (this->controller != nullptr)
	{
		SDL_GameControllerClose(controller);
	}
}

void GamePad::updateButtonState(GAMEPAD::XBOX_360::BUTTON button, bool state)
{
	this->buttonStateMap[button] = state;
	this->buttonStateTickMap[button] = state;
}

void GamePad::updateAxisValue(GAMEPAD::XBOX_360::AXIS axis, float value)
{
	this->axisValueMap[axis] = value;
}

const float GamePad::convertAxisValue(GamePadID rawValue, const float modifier)
{
	float value = 0;

	if (rawValue < AXIS_MIN)
	{
		value = static_cast<float>(rawValue) / SDL_AXIS_MIN_ABS_VALUE;
	}
	else if (rawValue > AXIS_MAX)
	{
		value = static_cast<float>(rawValue) / SDL_AXIS_MAX_ABS_VALUE;
	}
	else
	{
		value = 0;
	}

	value *= (modifier);

	return value;
}

void GamePad::playRumble(const float strength, const Uint32 length)
{
	if (hapticEnabled)
	{
		if (haptic)
		{
			SDL_HapticRumbleStop(this->haptic);
			SDL_HapticRumblePlay(this->haptic, strength * hapticModifier, length);
		}
	}
}

bool Voxel::GamePad::isButtonPressed(GAMEPAD::XBOX_360::BUTTON button, const bool tick)
{
	if (tick)
	{
		auto find_key = buttonStateTickMap.find(button);
		if (find_key != buttonStateTickMap.end())
		{
			return (find_key->second) == true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return buttonStateMap[button] == true;
	}
}

bool Voxel::GamePad::isButtonReleased(GAMEPAD::XBOX_360::BUTTON button, const bool tick)
{
	if (tick)
	{
		auto find_key = buttonStateTickMap.find(button);
		if (find_key != buttonStateTickMap.end())
		{
			return (find_key->second) == false;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return buttonStateMap[button] == false;
	}
}

float Voxel::GamePad::getAxisValue(GAMEPAD::XBOX_360::AXIS axis)
{
	return axisValueMap[axis];
}

void Voxel::GamePad::postUpdate()
{
	buttonStateTickMap.clear();
}

GamePadManager::GamePadManager()
{
	//Initialize SDL
	if (SDL_Init(SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC) < 0)
	{
		auto logger = &Voxel::Logger::getInstance();

		logger->error("[System] Error: " + std::to_string(Voxel::Error::Code::ERROR_FAILED_TO_INITIALIZE_SDL));
		active = false;
	}
	else
	{
#if V_DEBUG && V_DEBUG_LOG_CONSOLE
		auto logger = &Voxel::Logger::getInstance();
		logger->consoleInfo("[GamePadManager] SDL2 is initialized with GAMECONTROLLER and HAPTIC");
#endif

		active = true;
	}
}

GamePadManager::~GamePadManager()
{
}

GamePadManager* GamePadManager::getInstance()
{
	if (instance == nullptr)
	{
		instance = new GamePadManager();
	}

	return instance;
}

void GamePadManager::deleteInstance()
{
	//check if instance is alive pointer
	if (instance != nullptr)
	{
		delete instance;
	}
}

void GamePadManager::update()
{
	if (!active) return;

	SDL_Event e;
	while (SDL_PollEvent(&e) != 0)
	{
		switch (e.type)
		{
		case SDL_CONTROLLERDEVICEADDED:
		{
			addController(e.cdevice);
		}
		break;
		case SDL_CONTROLLERDEVICEREMOVED:
		{
			removeController(e.cdevice);
		}
		break;
		case SDL_CONTROLLERBUTTONDOWN:
		{
			buttonPressed(e.cdevice.which, e.cbutton);
		}
		break;
		case SDL_CONTROLLERBUTTONUP:
		{
			buttonReleased(e.cdevice.which, e.cbutton);
		}
		break;
		case SDL_CONTROLLERAXISMOTION:
		{
			axisMoved(e.cdevice.which, e.caxis);
		}
		break;
		default:
			break;
		}
	}
}

void GamePadManager::addController(const SDL_ControllerDeviceEvent event)
{
	if (!active) return;

	if (SDL_IsGameController(event.which)) {
		// Get as controller
		SDL_GameController* newController = SDL_GameControllerOpen(event.which);

		if (newController != nullptr) {
			// Convert to joystick
			SDL_Joystick *joy = SDL_GameControllerGetJoystick(newController);
			// Get instance id
			const GamePadID instanceID = SDL_JoystickInstanceID(joy);

			// Check duplication
			auto find_it = this->gamePads.find(instanceID);
			if (find_it == this->gamePads.end())
			{
				// Get number of button and axis on controller
				const int buttonCount = SDL_JoystickNumButtons(joy);
				const int axisCount = SDL_JoystickNumAxes(joy);
				// Get name
				std::string name = std::string(SDL_JoystickName(joy));

#if V_DEBUG && V_DEBUG_LOG_CONSOLE
				auto logger = &Voxel::Logger::getInstance();

				logger->consoleInfo("[GamePadManager] Gamepad is added");
				logger->consoleInfo("[GamePadManager] ID: " + std::to_string(instanceID));
				logger->consoleInfo("[GamePadManager] Name: " + name);
				logger->consoleInfo("[GamePadManager] Button count: " + std::to_string(buttonCount));
				logger->consoleInfo("[GamePadManager] Axis count: " + std::to_string(axisCount));
#endif

				SDL_Haptic* newHaptic = nullptr;

				bool hasHaptic = (SDL_JoystickIsHaptic(joy) == 1);
				if (hasHaptic)
				{
					newHaptic = SDL_HapticOpenFromJoystick(joy);

#if V_DEBUG && V_DEBUG_LOG_CONSOLE
					auto logger = &Voxel::Logger::getInstance();
					logger->consoleInfo("[GamePadManager] Haptic support: Y\n");
#endif
				}
				else
				{
					newHaptic = nullptr;

#if V_DEBUG && V_DEBUG_LOG_CONSOLE
					auto logger = &Voxel::Logger::getInstance();
					logger->consoleInfo("[GamePadManager] Haptic support: N\n");
#endif
				}

				this->gamePads[instanceID] = new GamePad(newController, newHaptic, name, instanceID);

				if (onControllerConnected)
				{
					onControllerConnected(instanceID);
				}
			}
			else
			{
				// Controller with same id already exists
				return;
			}
		}
		else
		{
			// New controller is invalid
			return;
		}
	}
}

void GamePadManager::removeController(const SDL_ControllerDeviceEvent event)
{
	if (!active) return;

	auto find_it = this->gamePads.find(event.which);

	if (find_it != this->gamePads.end())
	{
		if (onControllerDisconnected)
		{
			onControllerDisconnected(event.which);
		}

#if V_DEBUG && V_DEBUG_LOG_CONSOLE
		auto logger = &Voxel::Logger::getInstance();
		logger->consoleInfo("[GamePadManager] Gamepad is removed");
		logger->consoleInfo("[GamePadManager] ID: " + std::to_string(event.which));
		logger->consoleInfo("[GamePadManager] Name: " + (find_it->second)->name);
#endif

		delete find_it->second;
	}

	this->gamePads[event.which] = nullptr;
}

void GamePadManager::buttonPressed(GamePadID id, const SDL_ControllerButtonEvent event)
{
	if (!active) return;

	if (event.state == SDL_PRESSED)
	{
		GAMEPAD::XBOX_360::BUTTON buttonEnum = static_cast<GAMEPAD::XBOX_360::BUTTON>(event.button);
		
		auto find_it = this->gamePads.find(id);
		if (find_it != this->gamePads.end())
		{
			(find_it->second)->updateButtonState(buttonEnum, true);
		}

		if (onButtonPressed)
		{
			onButtonPressed(id, buttonEnum);
		}
	}
}

void GamePadManager::buttonReleased(GamePadID id, const SDL_ControllerButtonEvent event)
{
	if (!active) return;

	if (event.state == SDL_RELEASED)
	{
		GAMEPAD::XBOX_360::BUTTON buttonEnum = static_cast<GAMEPAD::XBOX_360::BUTTON>(event.button);

		auto find_it = this->gamePads.find(id);
		if (find_it != this->gamePads.end())
		{
			(find_it->second)->updateButtonState(buttonEnum, false);
		}

		if (onButtonReleased)
		{
			onButtonReleased(id, buttonEnum);
		}
	}
}

void GamePadManager::axisMoved(GamePadID id, const SDL_ControllerAxisEvent event)
{
	if (!active) return;

	GamePad* controller = findGamePad(id);
	if (controller == nullptr) { return; }

	GAMEPAD::XBOX_360::AXIS axis = static_cast<GAMEPAD::XBOX_360::AXIS>(event.axis);
	GamePadID value = event.value;

	// Modifier determine whether axis is x or y. x = 1.0, y = -1.0
	float modifier = 0;
	float newValue = 0;

	if (axis == GAMEPAD::XBOX_360::AXIS::L_AXIS_X || axis == GAMEPAD::XBOX_360::AXIS::R_AXIS_X)
	{
		modifier = 1.0f;
		newValue = controller->convertAxisValue(value, modifier);
	}
	else if (axis == GAMEPAD::XBOX_360::AXIS::L_AXIS_Y || axis == GAMEPAD::XBOX_360::AXIS::R_AXIS_Y)
	{
		modifier = -1.0f;
		newValue = controller->convertAxisValue(value, modifier);
	}

	if (axis == GAMEPAD::XBOX_360::AXIS::LT || axis == GAMEPAD::XBOX_360::AXIS::RT)
	{
		newValue = controller->convertAxisValue(value);
	}

	controller->updateAxisValue(axis, newValue);

	if (newValue != 0)
	{
		if (onAxisMoved)
		{
			onAxisMoved(id, axis, newValue);
		}
	}
}

Sint16 GamePadManager::getMinAxisValue(GamePadID id) const
{
	if (!active) return 0;

	GamePad* gamePad = findGamePad(id);
	if (gamePad != nullptr)
	{
		return gamePad->AXIS_MIN;
	}
	else
	{
		return 0;
	}
}

void GamePadManager::setMinAxisValue(GamePadID id, Sint16 value)
{
	if (!active) return;

	GamePad* gamePad = findGamePad(id);
	if (gamePad != nullptr)
	{
		gamePad->AXIS_MIN = value;
	}
}

Sint16 GamePadManager::getMaxAxisValue(GamePadID id) const
{
	if (!active) return 0;

	GamePad* gamePad = findGamePad(id);
	if (gamePad != nullptr)
	{
		return gamePad->AXIS_MAX;
	}
	else
	{
		return 0;
	}
}

void GamePadManager::setMaxAxisValue(GamePadID id, Sint16 value)
{
	if (!active) return;

	GamePad* gamePad = findGamePad(id);
	if (gamePad != nullptr)
	{
		gamePad->AXIS_MAX = value;
	}
}

GamePad* GamePadManager::findGamePad(GamePadID id) const
{
	if (!active) return nullptr;

	auto find_it = this->gamePads.find(id);
	if (find_it != this->gamePads.end())
	{
		return (find_it->second);
	}
	else
	{
		return nullptr;
	}
}

float GamePadManager::getHapticModifier(GamePadID id) const
{
	if (!active) return 0.0f;

	GamePad* gamePad = findGamePad(id);
	if (gamePad != nullptr)
	{
		return gamePad->hapticModifier;
	}

	return 0.0f;
}

void GamePadManager::setHapticModifier(GamePadID id, float modifier)
{
	if (!active) return;

	GamePad* gamePad = findGamePad(id);
	if (gamePad != nullptr)
	{
		if (modifier < 0)
		{
			modifier = 0;
		}

		if (modifier > 2.0f)
		{
			modifier = 2.0f;
		}

		gamePad->hapticModifier = modifier;
	}
}

void GamePadManager::playRumble(GamePadID id, float strength, Uint32 length)
{
	if (!active) return;

	GamePad* gamePad = findGamePad(id);
	if (gamePad != nullptr)
	{
		if (strength < 0)
		{
			strength = 0;
		}

		if (length < 0)
		{
			length = 0;
		}

		gamePad->playRumble(strength, length);
	}
}

float Voxel::GamePadManager::getAxisValue(GamePadID id, GAMEPAD::XBOX_360::AXIS axis)
{
	if (!active) return 0.0f;

	GamePad* gamePad = findGamePad(id);
	if (gamePad != nullptr)
	{
		return gamePad->getAxisValue(axis);
	}
	else
	{
		return 0.0f;
	}
}

int Voxel::GamePadManager::getControllerCount()
{
	if (!active) return 0;

	return static_cast<int>(gamePads.size());
}

bool Voxel::GamePadManager::hasController()
{
	if (!active) return false;

	return getControllerCount() > 0;
}

bool GamePadManager::isButtonPressed(GamePadID id, GAMEPAD::XBOX_360::BUTTON button, const bool tick) const
{
	if (!active) return false;

	// get gamepad
	GamePad* gamepad = findGamePad(id);

	if (gamepad != nullptr)
	{
		return gamepad->isButtonPressed(button, tick);
	}

	// failed
	return false;
}

bool GamePadManager::isButtonReleased(GamePadID id, GAMEPAD::XBOX_360::BUTTON button, const bool tick) const
{
	if (!active) return false;

	// get gamepad
	GamePad* gamepad = findGamePad(id);

	if (gamepad != nullptr)
	{
		return gamepad->isButtonReleased(button, tick);
	}

	// failed
	return false;
}

bool GamePadManager::isAxisMoved(GamePadID id, GAMEPAD::XBOX_360::AXIS axis) const
{
	if (!active) return false;

	GamePad* controller = findGamePad(id);
	if (controller != nullptr)
	{
		return controller->axisValueMap[axis] != 0;
	}

	return false;
}

bool GamePadManager::hasHaptic(GamePadID id) const
{
	if (!active) return false;

	GamePad* controller = findGamePad(id);
	if (controller != nullptr)
	{
		return (controller->haptic != nullptr);
	}
	else
	{
		return false;
	}
}