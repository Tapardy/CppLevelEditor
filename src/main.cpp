#include <raylib.h>
#include <iostream>
#include <vector>
#include "typedef.h"
#include "Inputs/inputs.h"
#include "LevelEditor/objectsUI.h"
#include "LevelEditor/gameEntity.h"
#include "SaveLevel/save.h"
#include "../imgui/imgui.h"
#include "../imgui/rlImGui.h"
#include "../imgui/rlImGuiColors.h"
#include <raymath.h>

int main()
{
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "3D Game raylib");

    rlImGuiSetup(true);

    Camera3D camera = {0};
    camera.position = Vector3{10.0f, 10.0f, 10.0f};
    camera.target = Vector3{0.0f, 0.0f, 0.0f};
    camera.up = Vector3{0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    RayCollision collision = {0};

    std::vector<GameEntity *> entities;
    GameEntity *selectedEntity = nullptr;

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

        //  Makes it so the selected cube isn't lost when clicking UI *made it impossible to modify cube data*
        bool isMouseOverImGui = ImGui::GetIO().WantCaptureMouse;

        // Really should extract this by now
        Ray mouseRay = GetScreenToWorldRay(GetMousePosition(), camera);

        if (!isMouseOverImGui && !IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                // Check if we clicked on a gizmo first
                bool clickedOnGizmo = false;
                if (selectedEntity)
                {
                    clickedOnGizmo = ObjectUI::IsGizmoClicked(camera, mouseRay);
                }

                if (!clickedOnGizmo)
                {
                    selectedEntity = nullptr;

                    for (auto entity : entities)
                    {
                        auto transform = entity->GetComponent<TransformComponent>();
                        if (!transform)
                            continue;

                        if (auto cube = entity->GetComponent<CubeComponent>())
                        {
                            BoundingBox box = cube->GetBoundingBox();
                            collision = GetRayCollisionBox(mouseRay, box);
                        }
                        else if (auto sphere = entity->GetComponent<SphereComponent>())
                        {
                            float scaledRadius = sphere->GetScaledRadius();
                            collision = GetRayCollisionSphere(mouseRay, transform->position, scaledRadius);
                        }
                        else
                        {
                            continue;
                        }

                        if (collision.hit)
                        {
                            selectedEntity = entity;
                            break;
                        }
                    }
                }
            }
        }
        // Indenting for readability
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
            {
                DrawGrid(50, 1.0f);

                if (selectedEntity && !IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
                {
                    ObjectUI::UpdateAndRenderGizmos(camera, selectedEntity, mouseRay);
                }

                for (auto entity : entities)
                {
                    auto transform = entity->GetComponent<TransformComponent>();
                    if (!transform)
                        continue;

                    if (auto cube = entity->GetComponent<CubeComponent>())
                    {
                        Vector3 scaledSize = cube->GetScaledSize();
                        DrawCubeV(transform->position, scaledSize, cube->color);

                        if (entity == selectedEntity)
                        {
                            DrawCubeWiresV(transform->position,
                                           Vector3{scaledSize.x + 0.2f, scaledSize.y + 0.2f, scaledSize.z + 0.2f},
                                           BLACK);
                        }
                    }
                    else if (auto sphere = entity->GetComponent<SphereComponent>())
                    {
                        float scaledRadius = sphere->GetScaledRadius();
                        DrawSphere(transform->position, scaledRadius, sphere->color);

                        if (entity == selectedEntity)
                        {
                            DrawSphereWires(transform->position, scaledRadius + 0.1f, 16, 16, BLACK);
                        }
                    }
                }
            }
            EndMode3D();

            // Last is better, will always have it on top
            rlImGuiBegin();
            {
                ObjectUI::RenderGeneralUI(&selectedEntity, entities);
            }
            rlImGuiEnd();
        }
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
}