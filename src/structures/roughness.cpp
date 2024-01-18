#include "structures/roughness.hpp"
#include "map/MapGenerator.hpp"

std::vector<float> genRoughness(unsigned int seed, unsigned int size, float scale)
{
    std::vector<float> perlinMapRugged = generatePerlinMap(seed, size, size, 16, 0.3f * scale, 5.0f, 1.0f, 0.8f);

    return perlinMapRugged;
}