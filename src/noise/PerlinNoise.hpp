#pragma once

#include <vector>

std::vector<float> generatePerlinNoise(unsigned int seed, unsigned int width, unsigned int height, float frequency, float amplitude);

std::vector<float> generateTilePerlinNoise(unsigned int seed, unsigned int width, unsigned int height, float frequency, float amplitude);