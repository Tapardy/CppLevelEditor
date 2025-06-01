#pragma once
#include <vector>
#include "gameEntity.h"
#include "gizmo.h"
#include "../typedef.h"

class ObjectUI
{
public:
    static void RenderGeneralUI(GameEntity **selectedEntity, std::vector<GameEntity *> &entities);
    static void RenderTransformComponentUI(TransformComponent *transform);
    static void RenderCubeComponentUI(CubeComponent *cube);
    static void RenderSphereComponentUI(SphereComponent *sphere);

    static void SetupGizmoSystem();
    static void UpdateAndRenderGizmos(Camera camera, GameEntity *selectedEntity, Ray mouseRay);
    static bool IsGizmoClicked(Camera camera, Ray mouseRay);
    bool IsGizmoHovered(Ray mouseRay);

private:
    static GizmoSystem gizmoSystem;
};