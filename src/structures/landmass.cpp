#include "structures/landmass.hpp"
#include "map/MapGenerator.hpp"

#include <algorithm>
#include <functional>

namespace
{
    float findPercentile(std::vector<float> perlinMap, float percentile)
    {
        std::sort(perlinMap.begin(), perlinMap.end());

        unsigned int index = perlinMap.size() * percentile;

        return perlinMap[index];
    }
}

std::vector<float> genLandmasses(unsigned int seed, unsigned int size, float scale, float waterPercentage)
{
    std::vector<float> perlinMapLandWater = generatePerlinMap(seed+3, size, size, 15, 0.005f * scale, 1.2f, 1.0f, 0.8f);
    
    float minLandmass = *std::min_element(perlinMapLandWater.begin(), perlinMapLandWater.end());

    std::transform(perlinMapLandWater.begin(), perlinMapLandWater.end(), perlinMapLandWater.begin(),
            std::bind(std::plus<float>(), std::placeholders::_1, -minLandmass));

    float maxLandmass = *std::max_element(perlinMapLandWater.begin(), perlinMapLandWater.end());

    std::transform(perlinMapLandWater.begin(), perlinMapLandWater.end(), perlinMapLandWater.begin(),
            std::bind(std::divides<float>(), std::placeholders::_1, maxLandmass));

    float percentile = findPercentile(perlinMapLandWater, waterPercentage);

    std::transform(perlinMapLandWater.begin(), perlinMapLandWater.end(), perlinMapLandWater.begin(),
        std::bind(std::plus<float>(), std::placeholders::_1, -percentile));

    std::transform(perlinMapLandWater.begin(), perlinMapLandWater.end(), perlinMapLandWater.begin(),
                [](float val) {return std::clamp(val, 0.0f, 1.0f);}
                );

    return perlinMapLandWater;
}