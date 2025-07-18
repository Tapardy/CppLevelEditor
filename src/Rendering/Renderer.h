#pragma once

#include <raylib.h>
#include <vector>
#include "../LevelEditor/gameEntity.h"

class Renderer
{
public:
    static void RenderComponents(const std::vector<GameEntity *> &entities, GameEntity *selectedEntity);
};
