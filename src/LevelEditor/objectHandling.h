#pragma once
#include "objects.h"
#include "objectsUI.h"
#include "../typedef.h"

namespace ObjectHandling
{
    void SpawnCube(vCubes &cubes, Object::Cube newCube);
    void ModifyCube(Object::Cube &cube, Vector3 newPosition, Vector3 newRotation, Vector3 newSize, Color newColor);
    void DeleteSelectedCube(vCubes &cubes, Object::Cube &selectedCube);
}