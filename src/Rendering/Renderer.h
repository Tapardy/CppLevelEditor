#pragma once

#include <raylib.h>
#include <vector>
#include "../LevelEditor/gameEntity.h"
#include "IRenderer.h"

class Renderer
{
public:
    static void SetActive(IRenderer *renderer);
    static IRenderer *Get();

    static void RenderComponents(const std::vector<GameEntity *> &entities, GameEntity *selectedEntity);

private:
    static IRenderer *s_ActiveRenderer;
};
