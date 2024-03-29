#include "structures/mountains.hpp"
#include "map/MapGenerator.hpp"

#include <algorithm>
#include <functional>

std::vector<float> genMountains(unsigned int seed, unsigned int size, std::array<int, 2> xyCenter, float scale) 
{

    // Mountains everywhere
    std::vector<float> perlinMapMountains = generatePerlinMap(seed+1, size, size, xyCenter[0], xyCenter[1], 10, 0.12f / scale, 2.0f, 1.0f, 0.55f);
  

    // To accentuate slopes, we square the map and then normalize it
    for (unsigned int i(0); i < 1; ++i)
    {
        std::transform(perlinMapMountains.begin(), perlinMapMountains.end(), perlinMapMountains.begin(),
                    perlinMapMountains.begin(), std::multiplies<float>());
    }


    return perlinMapMountains;
}