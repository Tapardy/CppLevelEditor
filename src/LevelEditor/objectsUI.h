#pragma once
#include <vector>
#include "gameEntity.h"
#include "gizmo.h"
#include "../typedef.h"

class ObjectUI
{
public:
    static void RenderGeneralUI(GameEntity **selectedEntity, std::vector<GameEntity *> &entities, GizmoSystem &gizmoSystem);
    static void RenderTransformComponentUI(GameEntity **selectedEntity, GizmoSystem &gizmoSystem);
    static void RenderCubeComponentUI(CubeComponent *cube);
    static void RenderSphereComponentUI(SphereComponent *sphere);
    static void RenderModelComponentUI(ModelComponent *model);

    static void UpdateAndRenderGizmos(Camera camera, GameEntity *selectedEntity, Ray mouseRay, GizmoSystem &gizmoSystem);
    static bool IsGizmoClicked(Camera camera, Ray mouseRay, GizmoSystem &gizmoSystem);

private:
    static GizmoSystem gizmoSystem;
};