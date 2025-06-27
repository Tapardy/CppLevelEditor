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
#include "../imgui/imguiStyle.h"
#include <raymath.h>
#include <rlgl.h>

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

                    // Apply world position first, then rotation/scale, order is important
                    rlPushMatrix();

                    rlTranslatef(transform->position.x, transform->position.y, transform->position.z);

                    Matrix transformMatrix = transform->GetTransformMatrix();
                    rlMultMatrixf(MatrixToFloat(transformMatrix));

                    if (auto cube = entity->GetComponent<CubeComponent>())
                    {
                        DrawCubeV(Vector3{0, 0, 0}, cube->size, cube->color);

                        if (entity == selectedEntity)
                        {
                            DrawCubeWiresV(Vector3{0, 0, 0},
                                           Vector3{cube->size.x + 0.02f, cube->size.y + 0.02f, cube->size.z + 0.02f},
                                           BLACK);
                        }
                    }
                    else if (auto sphere = entity->GetComponent<SphereComponent>())
                    {
                        DrawSphere(Vector3{0, 0, 0}, sphere->radius, sphere->color);

                        if (entity == selectedEntity)
                        {
                            DrawSphereWires(Vector3{0, 0, 0}, sphere->radius + 0.01f, 16, 16, BLACK);
                        }
                    }

                    rlPopMatrix();
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