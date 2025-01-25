#pragma once
#include <vector>
#include "objects.h"
#include "src/typedef.h"

namespace ObjectUI
{
    Object::Cube SpawnCube(vCubes &cubes, Object::Cube newCube);
    void RenderCubeGUI(vCubes &cubes, Object::Cube &selectedCube);
}
