#pragma once

#include <raylib.h>
#include <unordered_set>
#include <vector>
#include "InputObserver.h"

class InputSystem
{
public:
    void RegisterObserver(InputObserver *observer);
    void UnregisterObserver(InputObserver *observer);
    void CheckInputs();

private:
    static bool IsControllerActive(int gamepadIndex);
    static int _gamepadIndex;

    std::unordered_set<int> currentlyPressedKeys;
    std::vector<InputObserver *> observers;
};
