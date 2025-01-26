#include <raylib.h>
#include "../../imgui/imgui.h"
#include "../../imgui/rlImGui.h"
#include "../typedef.h"
#include "objectsUI.h"
#include "objects.h"
#include "objectHandling.h"
#include <vector>
#include <string>

void ObjectUI::RenderCubeGUI(vCubes &cubes, Object::Cube *selectedCube)
{
    ImGui::Begin("Cube Editor");

    static float position[3] = {0, 0, 0};
    static float rotation[3] = {0, 0, 0};
    static float size[3] = {1, 1, 1};
    static Color color = RED;

    if (selectedCube != nullptr)
    {
        position[0] = selectedCube->position.x;
        position[1] = selectedCube->position.y;
        position[2] = selectedCube->position.z;

        rotation[0] = selectedCube->rotation.x;
        rotation[1] = selectedCube->rotation.y;
        rotation[2] = selectedCube->rotation.z;

        size[0] = selectedCube->size.x;
        size[2] = selectedCube->size.y;
        size[2] = selectedCube->size.z;

        color = selectedCube->color;
    }

    ImGui::InputFloat3("Position", position);
    ImGui::InputFloat3("Rotation", rotation);
    ImGui::InputFloat3("Size", size);
    ImGui::ColorEdit3("Cube Color", (float *)&color);

    if (selectedCube != nullptr)
    {
        selectedCube->position.x = position[0];
        selectedCube->position.y = position[1];
        selectedCube->position.z = position[2];

        selectedCube->rotation.x = rotation[0];
        selectedCube->rotation.y = rotation[1];
        selectedCube->rotation.z = rotation[2];

        selectedCube->size.x = size[0];
        selectedCube->size.y = size[1];
        selectedCube->size.z = size[2];

        selectedCube->color = color;
    }

    if (ImGui::Button("Spawn Cube"))
    {
        Vector3 newPosition = {position[0], position[1], position[2]};
        Vector3 newRotation = {rotation[0], rotation[1], rotation[2]};
        Vector3 newSize = {size[0], size[1], size[2]};
        Color newColor = color;

        Object::Cube newCube = {newPosition, newRotation, newSize, newColor};
        ObjectHandling::SpawnCube(cubes, newCube);
    }

    ImGui::End();
}