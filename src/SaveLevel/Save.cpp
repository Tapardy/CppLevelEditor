#include "src/LevelEditor/Objects.h"
#include <iostream>

bool SaveLevel(Object::Cube *cubes, int cubeCount)
{
    MakeDirectory("Levels");

    return SaveFileData("Levels/level.dat", cubes, cubeCount * sizeof(Object::Cube));
}

void LoadLevel(Object::Cube *cubes, int &cubeCount)
{
    int dataSize;
    unsigned char *data = LoadFileData("Levels/level.dat", &dataSize);

    cubeCount = dataSize / sizeof(Object::Cube);

    memcpy(cubes, data, dataSize);

    UnloadFileData(data);
}
