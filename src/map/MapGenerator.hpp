#pragma once

#include <vector>

std::vector<float> generatePerlinMap(unsigned int seed, unsigned int width, unsigned int heigh, unsigned int octaves, float initialFrequency, float lacunarity, float initialAmplitude, float persistence);