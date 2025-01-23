#ifndef __INPUTS_H__
#define __INPUTS_H__

#include <raylib.h>

// Reusable system that will allow me to map multiple inputs to a single call, like godot or old unity input system (kinda)
class InputSystem
{
public:
    static bool Jump();
    static bool Run();
    static Vector2 GetMovementDirection();

private:
    static bool IsControllerActive(int gamepadIndex);
    static int _gamepadIndex;
};

#endif