#include "objectHandling.h"
#include "objects.h"
#include "objectsUI.h"
#include "../typedef.h"

Object::Cube ObjectHandling::SpawnCube(vCubes &cubes, Object::Cube newCube)
{
    cubes.push_back(Object::Cube::CreateCube(
        newCube.position,
        newCube.rotation,
        newCube.size,
        newCube.color));

    return cubes.back();
}
