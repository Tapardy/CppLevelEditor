#include "objectHandling.h"
#include "objects.h"
#include "objectsUI.h"
#include "../typedef.h"

void ObjectHandling::SpawnCube(vCubes &cubes, Object::Cube newCube)
{
    cubes.push_back(Object::Cube::CreateCube(
        newCube.position,
        newCube.rotation,
        newCube.size,
        newCube.color));
}

void ObjectHandling::ModifyCube(Object::Cube &cube, Vector3 newPosition, Vector3 newRotation, Vector3 newSize, Color newColor)
{
    cube.position = newPosition;
    cube.rotation = newRotation;
    cube.size = newSize;
    cube.color = newColor;
}

void ObjectHandling::DeleteSelectedCube(vCubes &cubes, Object::Cube &selectedCube)
{
    for (auto &cube : cubes)
    {
        if (&cube == &selectedCube)
        {
            cube = std::move(cubes.back());
            cubes.pop_back();
            break;
        }
    }
}