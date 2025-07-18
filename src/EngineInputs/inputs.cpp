#include "inputs.h"
#include <raylib.h>
#include <raymath.h>
#include "../Logging/Logger.h"

int InputSystem::_gamepadIndex = 0;
std::vector<InputObserver *> observers;

void InputSystem::RegisterObserver(InputObserver *observer)
{
    DebugPrint("Observer registered");
    observers.push_back(observer);
}

void InputSystem::UnregisterObserver(InputObserver *observer)
{
    DebugPrint("Observer unregistered");
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

void InputSystem::CheckInputs()
{
    int key = GetKeyPressed();
    while (key != 0)
    {
        DebugPrint("Key: ", key);
        for (auto &observer : observers)
        {
            if (IsKeyPressed(key))
            {
                observer->OnInputEvent(key, InputType::KeyPressed);
            }
            else if (IsKeyReleased(key))
            {
                observer->OnInputEvent(key, InputType::KeyReleased);
            }
            else if (IsKeyDown(key))
            {
                observer->OnInputEvent(key, InputType::KeyDown);
            }
            else if (IsKeyUp(key))
            {
                observer->OnInputEvent(key, InputType::IsKeyUp);
            }
        }

        key = GetKeyPressed();
    }
}

// Just keeping this here in case I ever need it
bool InputSystem::IsControllerActive(int gamepadIndex = 0)
{

    _gamepadIndex = gamepadIndex;
    return IsGamepadAvailable(gamepadIndex) &&
           (GetGamepadAxisCount(gamepadIndex) > 0);
}
