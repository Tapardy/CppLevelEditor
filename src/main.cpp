#include <raylib.h>
#include "Inputs/Inputs.h"
#include <iostream>

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

    Vector3 cubePosition = {0, 0, 0};

    DisableCursor();

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        UpdateCamera(&camera, CAMERA_FREE);

        // Indenting for readability
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            if (InputSystem::Jump())
                std::cout << "Jumped";
            BeginMode3D(camera);
            {

                DrawCube(cubePosition, 10, 10, 10, RED);
            }
            EndMode3D();
        }
        EndDrawing();
    }

    CloseWindow();
}