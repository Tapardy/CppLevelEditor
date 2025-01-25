#pragma once
#include "../LevelEditor/Objects.h"
#include <vector>

bool SaveLevel(std::vector<Object::Cube> *cubes);
void LoadLevel(std::vector<Object::Cube> *cubes);