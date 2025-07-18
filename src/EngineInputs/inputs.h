#pragma once

#include <raylib.h>
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
    std::vector<InputObserver *> observers;
};
