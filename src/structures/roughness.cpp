#include "structures/roughness.hpp"
#include "map/MapGenerator.hpp"

std::vector<float> genRoughness(unsigned int seed, unsigned int size, std::array<int, 2> xyCenter, float scale)
{
    std::vector<float> perlinMapRugged = generatePerlinMap(seed, size, size, xyCenter[0], xyCenter[1], 16, 0.3f / scale, 5.0f, 1.0f, 0.8f);

    return perlinMapRugged;
}