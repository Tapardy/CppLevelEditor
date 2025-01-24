#include <raylib.h>
#include <iostream>
#include "Inputs/Inputs.h"
#include "LevelEditor/Objects.h"

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

    Object::Cube cubes[2] = {
        {{0, 0, 0}, {0, 0, 0}, {2, 2, 2}, RED},
        {{4, 8, 4}, {0, 0, 0}, {2, 2, 2}, GREEN},
    };

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
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
                }
            }
            else
            {
                collision.hit = false;
            }
        }

        // Indenting for readability
        BeginDrawing();
        {
            for (auto &&cube : cubes)
            {
                if (collision.hit)
                    cube.color = ORANGE;
                else
                    cube.color = RED;
            }

            ClearBackground(RAYWHITE);

            if (InputSystem::Jump())
                std::cout << "Jumped";
            BeginMode3D(camera);
            {
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