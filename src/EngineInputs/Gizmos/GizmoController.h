#pragma once

#include "../../LevelEditor/Gizmo.h"
#include "../InputObserver.h"

class GizmoController : public InputObserver
{
public:
    GizmoController(GizmoSystem &gizmoSys);
    void OnInputEvent(int key, InputType type) override;

private:
    GizmoSystem &gizmoSystem;
};
