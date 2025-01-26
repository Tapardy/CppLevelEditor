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
    if (selectedCube != nullptr)
    {
        position[0] = selectedCube->position.x;
        position[1] = selectedCube->position.y;
        position[2] = selectedCube->position.z;
    }

    ImGui::InputFloat3("Position", position);

    if (selectedCube != nullptr)
    {
        selectedCube->position.x = position[0];
        selectedCube->position.y = position[1];
        selectedCube->position.z = position[2];
    }

    if (ImGui::Button("Spawn Cube"))
    {
        Vector3 newPosition = {position[0], position[1], position[2]};
        Object::Cube newCube = {newPosition, {0, 0, 0}, {2, 2, 2}, RED};
        ObjectHandling::SpawnCube(cubes, newCube);
    }

    ImGui::End();
}