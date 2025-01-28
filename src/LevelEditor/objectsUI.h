#pragma once
#include <vector>
#include "objects.h"
#include "gameEntity.h"
#include "../typedef.h"

namespace ObjectUI
{
    void RenderGeneralUI(GameEntity **selectedEntity, std::vector<GameEntity *> &entities);
    void RenderTransformComponentUI(TransformComponent *transform);
    void RenderCubeComponentUI(CubeComponent *cube);
    void RenderSphereComponentUI(SphereComponent *sphere);
}
