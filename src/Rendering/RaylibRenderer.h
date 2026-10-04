#pragma once

#include "IRenderer.h"
#include <raylib.h>

class RaylibRenderer : public IRenderer
{
public:
    RaylibRenderer();
    ~RaylibRenderer() override;

    void Init(int width, int height) override;
    void Shutdown() override;
    void ResizeViewport(int width, int height) override;

    void BeginScene(const Camera3D &camera) override;
    void EndScene() override;

    void SetDepthTest(bool enable) override;

    void DrawGrid(int slices, float spacing) override;
    void DrawCube(Vector3 position, Vector3 size, Color color, bool wireframe = false) override;
    void DrawSphere(Vector3 position, float radius, Color color, bool wireframe = false) override;
    void DrawCylinder(Vector3 start, Vector3 end, float startRadius, float endRadius, int sides, Color color) override;

    void DrawModel(const Model &model, Vector3 position, float scale, Color tint, const Matrix &transform) override;
    void RenderEntities(const std::vector<GameEntity *> &entities, GameEntity *selectedEntity) override;

    void *GetSceneTextureID() override;
    RenderTexture2D GetSceneTarget() const { return sceneTarget; }

private:
    RenderTexture2D sceneTarget = {0};
    int viewportWidth = 1920;
    int viewportHeight = 1080;
    bool isInitialized = false;

    RenderTexture2D LoadDepthTexture(int width, int height);
    void UnloadDepthTexture(RenderTexture2D target);
};
