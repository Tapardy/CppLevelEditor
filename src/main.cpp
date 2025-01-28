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

    Ray ray = {0};
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
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !isMouseOverImGui)
        {
            ray = GetScreenToWorldRay(GetMousePosition(), camera);
            selectedEntity = nullptr;
            for (auto entity : entities)
            {
                // Just skip without a transform
                auto transform = entity->GetComponent<TransformComponent>();
                if (!transform)
                    continue;

                if (auto cube = entity->GetComponent<CubeComponent>())
                {
                    // Have to do this again atm, will fix later
                    BoundingBox box = {
                        Vector3Subtract(transform->position, Vector3Scale(cube->size, 0.5f)),
                        Vector3Add(transform->position, Vector3Scale(cube->size, 0.5f))};
                    collision = GetRayCollisionBox(ray, box);
                }
                else if (auto sphere = entity->GetComponent<SphereComponent>())
                {
                    collision = GetRayCollisionSphere(ray, transform->position, sphere->radius);
                }
                else
                {
                    continue;
                }

                if (collision.hit)
                {
                    selectedEntity = entity;
                    // Break so you don't add another
                    break;
                }
            }
        }

        // Indenting for readability
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            rlImGuiBegin();
            {
                ObjectUI::RenderGeneralUI(&selectedEntity, entities);
            }
            rlImGuiEnd();

            BeginMode3D(camera);
            {
                for (auto entity : entities)
                {
                    auto transform = entity->GetComponent<TransformComponent>();
                    if (!transform)
                        continue;

                    // Check for CubeComponent
                    if (auto cube = entity->GetComponent<CubeComponent>())
                    {
                        DrawCubeV(transform->position, cube->size, cube->color);
                        if (entity == selectedEntity)
                        {
                            DrawCubeWiresV(transform->position, Vector3{cube->size.x + 0.2f, cube->size.y + 0.2f, cube->size.z + 0.2f}, BLACK);
                        }
                    }
                    // Check for SphereComponent
                    else if (auto sphere = entity->GetComponent<SphereComponent>())
                    {
                        DrawSphere(transform->position, sphere->radius, sphere->color);
                        if (entity == selectedEntity)
                        {
                            DrawSphereWires(transform->position, sphere->radius + 0.1f, 16, 16, BLACK);
                        }
                    }
                }
            }
            EndMode3D();
        }
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
}