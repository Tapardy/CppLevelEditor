#pragma once
#include "../LevelEditor/Objects.h"

void SaveLevel(Object::Cube *cubes, int cubeCount);
void LoadLevel(Object::Cube *cubes, int &cubeCount);