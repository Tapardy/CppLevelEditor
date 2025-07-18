#include "GizmoController.h"
#include "raylib.h"
#include "../InputType.h"
#include "../Logging/Logger.h"
#include <iostream>

GizmoController::GizmoController(GizmoSystem &gizmoSys)
    : gizmoSystem(gizmoSys) {}

void GizmoController::OnInputEvent(int key, InputType type)
{
    if (key == KEY_Q && type == InputType::KeyPressed)
        gizmoSystem.SetMode(GizmoMode::NONE);
    if (key == KEY_W && type == InputType::KeyPressed)
        gizmoSystem.SetMode(GizmoMode::POSITION);
    if (key == KEY_E && type == InputType::KeyPressed)
        gizmoSystem.SetMode(GizmoMode::ROTATION);
    if (key == KEY_R && type == InputType::KeyPressed)
        gizmoSystem.SetMode(GizmoMode::SCALE);
}
