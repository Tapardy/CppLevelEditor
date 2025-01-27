#include <raylib.h>
#include "../../imgui/imgui.h"
#include "../../imgui/rlImGui.h"
#include "../../imgui/rlImGuiColors.h"
#include "../typedef.h"
#include "objectsUI.h"
#include "objects.h"
#include "objectHandling.h"
#include <vector>
#include <string>

namespace ObjectUI
{
    void RenderCubeGUI(vCubes &cubes, Object::Cube *selectedCube)
    {
        ImGui::Begin("Cube Editor");

        static float position[3] = {0, 0, 0};
        static float rotation[3] = {0, 0, 0};
        static float size[3] = {1, 1, 1};
        static ImVec4 imguiColor = rlImGuiColors::Convert(RED);
        static Color color = RED;

        // Recieve the data
        if (selectedCube != nullptr)
        {
            position[0] = selectedCube->position.x;
            position[1] = selectedCube->position.y;
            position[2] = selectedCube->position.z;

            rotation[0] = selectedCube->rotation.x;
            rotation[1] = selectedCube->rotation.y;
            rotation[2] = selectedCube->rotation.z;

            size[0] = selectedCube->size.x;
            size[1] = selectedCube->size.y;
            size[2] = selectedCube->size.z;

            imguiColor = rlImGuiColors::Convert(selectedCube->color);
        }

        ImGui::InputFloat3("Position", position);
        ImGui::InputFloat3("Rotation", rotation);
        ImGui::InputFloat3("Size", size);
        ImGui::ColorEdit4("Cube Color", (float *)&imguiColor, ImGuiColorEditFlags_Float);

        color = rlImGuiColors::Convert(imguiColor);

        Vector3 newPosition = {position[0], position[1], position[2]};
        Vector3 newRotation = {rotation[0], rotation[1], rotation[2]};
        Vector3 newSize = {size[0], size[1], size[2]};

        // Modify the data
        if (selectedCube != nullptr)
        {
            ObjectHandling::ModifyCube(*selectedCube, newPosition, newRotation, newSize, color);
        }

        if (ImGui::Button("Spawn Cube"))
        {
            Object::Cube newCube = {newPosition, newRotation, newSize, color};
            ObjectHandling::SpawnCube(cubes, newCube);
        }

        if (ImGui::Button("Delete Cube") && selectedCube != nullptr)
        {
            ObjectHandling::DeleteSelectedCube(cubes, *selectedCube);
        }

        ImGui::End();
    }
}