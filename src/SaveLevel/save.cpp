#include "src/LevelEditor/Objects.h"
#include <iostream>
#include <vector>
#include "save.h"

bool SaveLevel(vCubes *cubes)
{
    MakeDirectory("Levels");

    return SaveFileData("Levels/level.dat", cubes->data(), cubes->size() * sizeof(Object::Cube));
}

void LoadLevel(vCubes *cubes)
{
    int dataSize;
    unsigned char *data = LoadFileData("Levels/level.dat", &dataSize);

    cubes->resize(dataSize / sizeof(Object::Cube));
    memcpy(cubes->data(), data, dataSize);

    UnloadFileData(data);
}