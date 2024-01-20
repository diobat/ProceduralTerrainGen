#pragma once

#include <vector>

std::vector<float> generatePerlinMap(unsigned int seed, unsigned int width, unsigned int height, int xCenter, int yCenter, unsigned int octaves, float initialFrequency, float lacunarity, float initialAmplitude, float persistence);

std::vector<float> genPM_FractalRidge(unsigned int seed, unsigned int width, unsigned int height, int xCenter, int yCenter, unsigned int octaves, float initialFrequency, float lacunarity, float initialAmplitude, float persistence);

std::vector<float> generateTilePerlinMap(unsigned int seed, unsigned int width, unsigned int heigh, unsigned int octaves, float initialFrequency, float lacunarity, float initialAmplitude, float persistence);