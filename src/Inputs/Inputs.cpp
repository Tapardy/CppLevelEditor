#include "Inputs.h"
#include <raylib.h>
#include <raymath.h>

int InputSystem::_gamepadIndex = 0;

bool InputSystem::Jump()
{
    if (IsKeyPressed(KEY_SPACE))
        return true;
    if (IsControllerActive(_gamepadIndex))
        if (IsGamepadButtonPressed(_gamepadIndex, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
            return true;
    return false;
}

bool InputSystem::Run()
{
    if (IsKeyDown(KEY_LEFT_SHIFT))
        return true;
    return false;
}

// Keeping this here, but maybe it's not needed, as raylib does have WASD and controller axis builtin on camera movement, which I did not know
Vector2 InputSystem::GetMovementDirection()
{
    Vector2 direction = {0, 0};

    // Keyboard input
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
        direction.y -= 1;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
        direction.y += 1;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
        direction.x -= 1;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        direction.x += 1;

    if (IsControllerActive(_gamepadIndex))
    {
        // Use analog here due to controller having a wider range
        float axisX = GetGamepadAxisMovement(_gamepadIndex, GAMEPAD_AXIS_LEFT_X);
        float axisY = GetGamepadAxisMovement(_gamepadIndex, GAMEPAD_AXIS_LEFT_Y);

        // Some deadzone so the player has lieway
        const float deadzone = 0.2f;
        if (fabsf(axisX) > deadzone)
            direction.x = axisX;
        if (fabsf(axisY) > deadzone)
            direction.y = axisY;
    }

    return Vector2Normalize(direction);
}

// Could still be handy to check for some various other commands like possible later implementations
bool InputSystem::IsControllerActive(int gamepadIndex = 0)
{
    _gamepadIndex = gamepadIndex;
    return IsGamepadAvailable(gamepadIndex) &&
           (GetGamepadAxisCount(gamepadIndex) > 0);
}
