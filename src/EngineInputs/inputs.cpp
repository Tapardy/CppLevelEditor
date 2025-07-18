#include "inputs.h"
#include <raylib.h>
#include <raymath.h>
#include "../Logging/Logger.h"
#include <unordered_set>

int InputSystem::_gamepadIndex = 0;

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
    std::unordered_set<int> newPressedKeys;

    int key = GetKeyPressed();
    while (key != 0)
    {
        // Prevent the keypressed to trigger when letting go of the key
        if (IsKeyDown(key))
        {
            DebugPrint("Key Pressed: ", key);
            newPressedKeys.insert(key);

            for (auto &observer : observers)
            {
                observer->OnInputEvent(key, InputType::KeyPressed);
            }
        }

        key = GetKeyPressed();
    }

    std::unordered_set<int> stillPressedKeys;

    for (int key : currentlyPressedKeys)
    {
        if (IsKeyDown(key))
        {
            stillPressedKeys.insert(key);
            for (auto &observer : observers)
            {
                observer->OnInputEvent(key, InputType::KeyDown);
            }
        }
        else
        {
            for (auto &observer : observers)
            {
                observer->OnInputEvent(key, InputType::KeyReleased);
                observer->OnInputEvent(key, InputType::IsKeyUp);
            }
        }
    }

    for (int key : newPressedKeys)
    {
        if (IsKeyDown(key))
        {
            stillPressedKeys.insert(key);
        }
    }

    currentlyPressedKeys = std::move(stillPressedKeys);
}

// Just keeping this here in case I ever need it
bool InputSystem::IsControllerActive(int gamepadIndex = 0)
{
    _gamepadIndex = gamepadIndex;
    return IsGamepadAvailable(gamepadIndex) &&
           (GetGamepadAxisCount(gamepadIndex) > 0);
}
