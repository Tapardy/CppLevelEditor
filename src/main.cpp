#include <raylib.h>
#include <rlgl.h>
#include <vector>
#include <iostream>

#include "typedef.h"
#include "EngineInputs\inputs.h"
#include "EngineInputs\Gizmos\GizmoController.h"
#include "LevelEditor/objectsUI.h"
#include "LevelEditor/gameEntity.h"
#include "SaveLevel/save.h"
#include "../imgui/imgui.h"
#include "../imgui/rlImGui.h"
#include "../imgui/rlImGuiColors.h"
#include "../imgui/imguiStyle.h"
#include "Rendering/Renderer.h"
#include "Logging/Logger.h"
#include "Logging/ConsoleUI.h"
#include <raymath.h>

// Link incase I forget how it works
// https://www.raylib.com/examples/shaders/loader.html?name=shaders_write_depth
static RenderTexture2D LoadRenderTextureDepthTex(int width, int height)
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
            TRACELOG(LOG_INFO, "FBO: [ID %i] Framebuffer object created successfully", target.id);

        rlDisableFramebuffer();
    }
    else
        TRACELOG(LOG_WARNING, "FBO: Framebuffer object can not be created");

    return target;
}

static void UnloadRenderTextureDepthTex(RenderTexture2D target)
{
    if (target.id > 0)
    {
        rlUnloadTexture(target.texture.id);
        rlUnloadTexture(target.depth.id);
        rlUnloadFramebuffer(target.id);
    }
}

int main()
{
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "3D Game raylib");

    rlImGuiSetup(true);
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    SetCustomImGuiStyle();

    Camera3D camera = {0};
    camera.position = Vector3{10.0f, 10.0f, 10.0f};
    camera.target = Vector3{0.0f, 0.0f, 0.0f};
    camera.up = Vector3{0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    RayCollision collision = {0};

    std::vector<GameEntity *> entities;
    GameEntity *selectedEntity = nullptr;

    GizmoSystem gizmoSystem;

    InputSystem inputSystem;
    GizmoController gizmoController(gizmoSystem);

    // Register controller as observer
    inputSystem.RegisterObserver(&gizmoController);

    RenderTexture2D sceneTarget = LoadRenderTextureDepthTex(screenWidth, screenHeight);

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {

        if (IsKeyPressed(KEY_F5))
        {
            // Need to implement the new Save function with the GameEntities, but lazy atm
        }
        if (IsKeyPressed(KEY_F6))
        {
            // Need to implement the new Load function with the GameEntities, but lazy atm
        }

        // Simulate a godot cam and make it much easier for me to move objects
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            UpdateCamera(&camera, CAMERA_FREE);
            DisableCursor();
        }
        else if (IsCursorHidden())
            EnableCursor();

        if (!io.WantTextInput)
        {
            inputSystem.CheckInputs();
        }

        bool isMouseOverImGui = ImGui::GetIO().WantCaptureMouse;
        Ray mouseRay = GetScreenToWorldRay(GetMousePosition(), camera);

        // TODO: Also extract this bit into a separate script for the same reason as renderer
        if (!isMouseOverImGui && !IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                // Check if we clicked on a gizmo first
                bool clickedOnGizmo = false;
                if (selectedEntity)
                    clickedOnGizmo = ObjectUI::IsGizmoClicked(camera, mouseRay, gizmoSystem);

                if (!clickedOnGizmo)
                {
                    selectedEntity = nullptr;
                    for (auto entity : entities)
                    {

                        if (auto cube = entity->GetComponent<CubeComponent>())
                        {
                            BoundingBox box = cube->GetBoundingBox();
                            collision = GetRayCollisionBox(mouseRay, box);
                        }
                        else if (auto sphere = entity->GetComponent<SphereComponent>())
                        {
                            float scaledRadius = sphere->GetScaledRadius();
                            collision = GetRayCollisionSphere(mouseRay, entity->EntityTransform.position, scaledRadius);
                        }
                        else
                            continue;

                        if (collision.hit)
                        {
                            selectedEntity = entity;
                            break;
                        }
                    }
                }
            }
        }

        if (selectedEntity && !IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            // Draw gizmos here, so it is synced to the object you're dragging, might change this to just update gizmos and render them below
            ObjectUI::UpdateAndRenderGizmos(camera, selectedEntity, mouseRay, gizmoSystem);
        }

        BeginTextureMode(sceneTarget);
        {
            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
            {
                DrawGrid(50, 1.0f);
                // Render components separately, as with many components this can bloat the file a lot
                Renderer::RenderComponents(entities, selectedEntity);
            }
            EndMode3D();
        }
        EndTextureMode();

        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            DrawTextureRec(
                sceneTarget.texture,
                Rectangle{0, 0, (float)screenWidth, (float)-screenHeight}, // Flip Y
                Vector2{0, 0},
                WHITE);

            BeginMode3D(camera);
            rlDisableDepthTest();
            if (selectedEntity && !IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
            {
                // Draw gizmos again, as otherwise they won't be on top
                ObjectUI::UpdateAndRenderGizmos(camera, selectedEntity, mouseRay, gizmoSystem);
            }
            rlEnableDepthTest();
            EndMode3D();

            rlImGuiBegin();
            ObjectUI::RenderGeneralUI(&selectedEntity, entities, gizmoSystem);
            // Test Print
            // DebugPrint("Test", selectedEntity);
            // DebugWarn("Test", selectedEntity);
            // DebugPrint(selectedEntity, "Test");
            // DebugWarn(selectedEntity, "Test");
            // DebugPrint(1);

            RenderConsoleUI(logBuffer);
            rlImGuiEnd();
        }
        EndDrawing();
    }

    rlImGuiShutdown();
    UnloadRenderTextureDepthTex(sceneTarget);
    CloseWindow();
}