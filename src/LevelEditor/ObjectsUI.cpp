#include "ObjectsUI.h"
#include "Objects.h"
#include <vector>

Object::Cube ObjectUI::SpawnCube(std::vector<Object::Cube> &cubes)
{
    // TODO: Implement labels to change spawn location
    cubes.push_back(Object::Cube::CreateCube({0, 0, 0}, {0, 0, 0}, {2, 2, 2}, RED));
    return cubes.back();
}
