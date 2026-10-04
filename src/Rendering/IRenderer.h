#pragma once

#include <raylib.h>
#include <vector>

class GameEntity;

class IRenderer
{
public:
    virtual ~IRenderer() = default;

    // init
    virtual void Init(int width, int height) = 0;
    virtual void Shutdown() = 0;
    virtual void ResizeViewport(int width, int height) = 0;

    // scene
    virtual void BeginScene(const Camera3D &camera) = 0;
    virtual void EndScene() = 0;

    // render
    virtual void SetDepthTest(bool enable) = 0;

    // debug
    virtual void DrawGrid(int slices, float spacing) = 0;
    virtual void DrawCube(Vector3 position, Vector3 size, Color color, bool wireframe = false) = 0;
    virtual void DrawSphere(Vector3 position, float radius, Color color, bool wireframe = false) = 0;
    virtual void DrawCylinder(Vector3 start, Vector3 end, float startRadius, float endRadius, int sides, Color color) = 0;

    // entity
    virtual void DrawModel(const Model &model, Vector3 position, float scale, Color tint, const Matrix &transform) = 0;
    virtual void RenderEntities(const std::vector<GameEntity *> &entities, GameEntity *selectedEntity) = 0;

    // texture n viewport for imgui
    virtual void *GetSceneTextureID() = 0;
};
