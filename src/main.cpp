#define RAYGUI_IMPLEMENTATION
#include <raylib.h>
#include <raygui.h>
#include <iostream>
#include <vector>
#include "Inputs/Inputs.h"
#include "LevelEditor/Objects.h"
#include "LevelEditor/ObjectsUI.h"
#include "SaveLevel/Save.h"

int main()
{
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "3D Game raylib");

    Camera3D camera = {0};
    camera.position = Vector3{10.0f, 10.0f, 10.0f};
    camera.target = Vector3{0.0f, 0.0f, 0.0f};
    camera.up = Vector3{0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Ray ray = {0};
    RayCollision collision = {0};

    // Apparently the vectors better than a list, so here we are
    std::vector<Object::Cube> cubes;
    cubes.push_back(Object::Cube::CreateCube({0, 0, 0}, {0, 0, 0}, {2, 2, 2}, RED));

    Object::Cube *selectedCube = nullptr;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_F5))
        {
            SaveLevel(&cubes);
        }
        if (IsKeyPressed(KEY_F6))
        {
            LoadLevel(&cubes);
        }

        // Simulate a godot cam and make it much easier for me to move objects
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            UpdateCamera(&camera, CAMERA_FREE);
            DisableCursor();
        }
        else if (IsCursorHidden())
            EnableCursor();

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            if (!collision.hit)
            {
                ray = GetScreenToWorldRay(GetMousePosition(), camera);
                for (auto &&cube : cubes)
                {
                    collision = GetRayCollisionBox(ray, cube.GetBoundingBox());
                    if (collision.hit)
                    {
                        selectedCube = &cube;
                        // Break so you don't add another
                        break;
                    }
                }
            }
            else
            {
                selectedCube = nullptr;
                collision.hit = false;
            }
        }

        if (selectedCube != nullptr)
        {
            Vector2 mouseDelta = GetMouseDelta();
            Vector2 scrollDirection = GetMouseWheelMoveV();

            selectedCube->position.x += mouseDelta.x * 0.05f;
            selectedCube->position.z += mouseDelta.y * 0.05f;

            // Not the best as you do easily move the mouse, but setting an if doesn't work, as theres intervals between scrolling
            selectedCube->position.y += scrollDirection.y * 0.2f;
        }

        // Indenting for readability
        BeginDrawing();
        {
            for (auto &&cube : cubes)
            {
                cube.color = (&cube == selectedCube) ? GREEN : RED;
            }

            ClearBackground(RAYWHITE);

            if (GuiButton({200, 200, 200, 100}, "Spawn Cube"))
            {
                ObjectUI::SpawnCube(cubes);
            }

            BeginMode3D(camera);
            {
                for (auto &&cube : cubes)
                {

                    DrawCubeV(cube.position, cube.size, cube.color);
                }
                for (auto &&cube : cubes)
                {
                    DrawCubeV(cube.position, cube.size, cube.color);
                }
            }
            EndMode3D();
        }
        EndDrawing();
    }

    CloseWindow();
}