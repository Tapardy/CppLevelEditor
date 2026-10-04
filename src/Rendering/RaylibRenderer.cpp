#include "RaylibRenderer.h"
#include <rlgl.h>
#include <raymath.h>
#include "../LevelEditor/gameEntity.h"

RaylibRenderer::RaylibRenderer()
{
}

RaylibRenderer::~RaylibRenderer()
{
    Shutdown();
}

// Link incase I forget how it works
// https://www.raylib.com/examples/shaders/loader.html?name=shaders_write_depth
RenderTexture2D RaylibRenderer::LoadDepthTexture(int width, int height)
{
    RenderTexture2D target = {0};
    target.id = rlLoadFramebuffer();
    if (target.id > 0)
    {
        rlEnableFramebuffer(target.id);

        target.texture.id = rlLoadTexture(0, width, height, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
        target.texture.width = width;
        target.texture.height = height;
        target.texture.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
        target.texture.mipmaps = 1;

        target.depth.id = rlLoadTextureDepth(width, height, false);
        target.depth.width = width;
        target.depth.height = height;
        target.depth.format = 19;
        target.depth.mipmaps = 1;

        rlFramebufferAttach(target.id, target.texture.id, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);
        rlFramebufferAttach(target.id, target.depth.id, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_TEXTURE2D, 0);

        if (rlFramebufferComplete(target.id))
            TRACELOG(LOG_INFO, "framebuffer [ID %i] created", target.id);

        rlDisableFramebuffer();
    }
    else
    {
        TRACELOG(LOG_WARNING, " Cant create framebuffer");
    }

    return target;
}

void RaylibRenderer::UnloadDepthTexture(RenderTexture2D target)
{
    if (target.id > 0)
    {
        rlUnloadTexture(target.texture.id);
        rlUnloadTexture(target.depth.id);
        rlUnloadFramebuffer(target.id);
    }
}

void RaylibRenderer::Init(int width, int height)
{
    viewportWidth = width;
    viewportHeight = height;
    sceneTarget = LoadDepthTexture(viewportWidth, viewportHeight);
    isInitialized = true;
}

void RaylibRenderer::Shutdown()
{
    if (isInitialized)
    {
        UnloadDepthTexture(sceneTarget);
        sceneTarget = {0};
        isInitialized = false;
    }
}

void RaylibRenderer::ResizeViewport(int width, int height)
{
    if (width <= 0 || height <= 0)
        return;
    if (width == viewportWidth && height == viewportHeight)
        return;

    Shutdown();
    Init(width, height);
}

void RaylibRenderer::BeginScene(const Camera3D &camera)
{
    BeginTextureMode(sceneTarget);
    ClearBackground(RAYWHITE);
    BeginMode3D(camera);
}

void RaylibRenderer::EndScene()
{
    EndMode3D();
    EndTextureMode();
}

void RaylibRenderer::SetDepthTest(bool enable)
{
    if (enable)
        rlEnableDepthTest();
    else
        rlDisableDepthTest();
}

void RaylibRenderer::DrawGrid(int slices, float spacing)
{
    ::DrawGrid(slices, spacing);
}

void RaylibRenderer::DrawCube(Vector3 position, Vector3 size, Color color, bool wireframe)
{
    if (wireframe)
        DrawCubeWiresV(position, size, color);
    else
        DrawCubeV(position, size, color);
}

void RaylibRenderer::DrawSphere(Vector3 position, float radius, Color color, bool wireframe)
{
    if (wireframe)
        DrawSphereWires(position, radius, 16, 16, color);
    else
        ::DrawSphere(position, radius, color);
}

void RaylibRenderer::DrawCylinder(Vector3 start, Vector3 end, float startRadius, float endRadius, int sides, Color color)
{
    DrawCylinderEx(start, end, startRadius, endRadius, sides, color);
}

void RaylibRenderer::DrawModel(const Model &model, Vector3 position, float scale, Color tint, const Matrix &transform)
{
    rlPushMatrix();
    rlMultMatrixf(MatrixToFloat(transform));
    ::DrawModel(model, position, scale, tint);
    rlPopMatrix();
}

void RaylibRenderer::RenderEntities(const std::vector<GameEntity *> &entities, GameEntity *selectedEntity)
{
    for (auto entity : entities)
    {
        rlPushMatrix();
        rlTranslatef(entity->EntityTransform.position.x, entity->EntityTransform.position.y, entity->EntityTransform.position.z);

        Matrix transformMatrix = entity->EntityTransform.GetTransformMatrix();
        rlMultMatrixf(MatrixToFloat(transformMatrix));

        if (auto cube = entity->GetComponent<CubeComponent>())
        {
            DrawCubeV(Vector3{0, 0, 0}, cube->size, cube->color);
            if (entity == selectedEntity)
                DrawCubeWiresV(Vector3{0, 0, 0}, Vector3{cube->size.x + 0.02f, cube->size.y + 0.02f, cube->size.z + 0.02f}, BLACK);
        }
        else if (auto sphere = entity->GetComponent<SphereComponent>())
        {
            ::DrawSphere(Vector3{0, 0, 0}, sphere->radius, sphere->color);
            if (entity == selectedEntity)
                DrawSphereWires(Vector3{0, 0, 0}, sphere->radius + 0.01f, 16, 16, BLACK);
        }
        else if (auto model = entity->GetComponent<ModelComponent>())
        {
            if (model->IsLoaded())
            {
                ::DrawModel(model->model, Vector3{0, 0, 0}, 1.0f, WHITE);
            }
        }

        rlPopMatrix();
    }
}

void *RaylibRenderer::GetSceneTextureID()
{
    return (void *)(intptr_t)sceneTarget.texture.id;
}
