#include "objectHandling.h"
#include "objects.h"
#include "objectsUI.h"

Object::Cube ObjectHandling::SpawnCube(std::vector<Object::Cube> &cubes, Object::Cube newCube)
{
    cubes.push_back(Object::Cube::CreateCube(
        newCube.position,
        newCube.rotation,
        newCube.size,
        newCube.color));

    return cubes.back();
}