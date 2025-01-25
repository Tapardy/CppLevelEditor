#pragma once
#include "../LevelEditor/objects.h"
#include <vector>

bool SaveLevel(std::vector<Object::Cube> *cubes);
void LoadLevel(std::vector<Object::Cube> *cubes);