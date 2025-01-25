#define RAYGUI_IMPLEMENTATION
#include <raylib.h>
#include <raygui.h>
#include "objectsUI.h"
#include "objects.h"
#include "objectHandling.h"
#include <vector>
#include <string>

void ObjectUI::RenderCubeGUI(std::vector<Object::Cube> &cubes, Object::Cube &selectedCube)
{
    static char posXText[40] = "0";
    static char posYText[40] = "0";
    static char posZText[40] = "0";

    static bool editModePosX = false;
    static bool editModePosY = false;
    static bool editModePosZ = false;

    GuiLabel({100, 100, 100, 100}, "PosX");
    GuiLabel({100, 100, 100, 100}, "PosX");
    GuiLabel({100, 100, 100, 100}, "PosX");

    if (GuiTextBox({160, 100, 40, 30}, posXText, 40, editModePosX))
    {
        editModePosX = !editModePosX;
    }
    if (GuiTextBox({200, 100, 40, 30}, posYText, 40, editModePosY))
    {
        editModePosX = !editModePosX;
    }
    if (GuiTextBox({240, 100, 40, 30}, posZText, 40, editModePosZ))
    {
        editModePosX = !editModePosX;
    }

    if (GuiButton({200, 150, 200, 100}, "Spawn Cube"))
    {
        float posX = std::stof(posXText);
        float posY = std::stof(posYText);
        float posZ = std::stof(posZText);

        Vector3 position = {posX, posY, posZ};

        Object::Cube newCube = {position, {0, 0, 0}, {2, 2, 2}, RED};
        ObjectHandling::SpawnCube(cubes, newCube);
    }
}
