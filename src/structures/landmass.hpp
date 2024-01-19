#pragma once

#include <vector>
#include <array>

std::vector<float> genLandmasses(unsigned int seed, unsigned int size, std::array<int, 2> xyCenter, float scale,  float waterPercentage = 0.7f);
