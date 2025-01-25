#define RAYGUI_IMPLEMENTATION
#include <raylib.h>
#include <raygui.h>
#include "../typedef.h"
#include "objectsUI.h"
#include "objects.h"
#include "objectHandling.h"
#include <vector>
#include <string>

void ObjectUI::RenderCubeGUI(vCubes &cubes, Object::Cube *selectedCube)
{
    GuiLabel({100, 100, 100, 30}, "Position");

    const char *axisLabels[3] = {"X", "Y", "Z"};
    static char posTexts[3][40] = {"0", "0", "0"};
    static bool editModes[3] = {false, false, false};

    if (selectedCube != nullptr)
    {
        snprintf(posTexts[0], sizeof(posTexts[0]), "%.2f", selectedCube->position.x);
        snprintf(posTexts[1], sizeof(posTexts[1]), "%.2f", selectedCube->position.y);
        snprintf(posTexts[2], sizeof(posTexts[2]), "%.2f", selectedCube->position.z);
    }

    for (int i = 0; i < 3; i++)
    {
        GuiLabel({160.0f + i * 80, 80, 40, 30}, axisLabels[i]);

        if (GuiTextBox({160.0f + i * 80, 100, 80, 30}, posTexts[i], 40, editModes[i]))
        {
            memset(editModes, 0, sizeof(editModes));
            editModes[i] = true;
        }
    }

    for (int i = 0; i < 3; i++)
    {
        if (selectedCube != nullptr)
        {
            selectedCube->position.x = std::stof(posTexts[0]);
            selectedCube->position.y = std::stof(posTexts[1]);
            selectedCube->position.z = std::stof(posTexts[2]);
        }
    }

    if (GuiButton({200, 150, 200, 50}, "Spawn Cube"))
    {
        Vector3 position = {
            std::stof(posTexts[0]),
            std::stof(posTexts[1]),
            std::stof(posTexts[2])};

        Object::Cube newCube = {position, {0, 0, 0}, {2, 2, 2}, RED};
        ObjectHandling::SpawnCube(cubes, newCube);
    }
}