#pragma once

#include "InputType.h"

class InputObserver
{
public:
    virtual ~InputObserver() = default;
    virtual void OnInputEvent(int key, const InputType type) = 0;
};