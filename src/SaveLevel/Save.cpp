#include "src/LevelEditor/Objects.h"
#include <iostream>
#include <vector>
#include "Save.h"

bool SaveLevel(std::vector<Object::Cube> *cubes)
{
    MakeDirectory("Levels");

    return SaveFileData("Levels/level.dat", cubes->data(), cubes->size() * sizeof(Object::Cube));
}

void LoadLevel(std::vector<Object::Cube> *cubes)
{
    int dataSize;
    unsigned char *data = LoadFileData("Levels/level.dat", &dataSize);

    cubes->resize(dataSize / sizeof(Object::Cube));

    memcpy(cubes->data(), data, dataSize);

    UnloadFileData(data);
}