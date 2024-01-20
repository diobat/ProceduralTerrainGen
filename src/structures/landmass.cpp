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

    float waterLevelOffset = 0.0f;

    void findOffset(const std::vector<float>& sample, float waterPercentage)
    {
        std::vector<float> sampleCopy(sample);

        waterLevelOffset = findPercentile(sampleCopy, waterPercentage);
    }

}

std::vector<float> genLandmasses(unsigned int seed, unsigned int size, std::array<int, 2> xyCenter, float scale, float waterPercentage)
{
    std::vector<float> perlinMapLandWater = generatePerlinMap(seed+3, size, size, xyCenter[0], xyCenter[1], 15, 0.005f * scale, 1.2f, 1.0f, 0.8f);
    
    if(waterLevelOffset == 0.0f)
    {
        findOffset(perlinMapLandWater, waterPercentage);
    }

    std::transform(perlinMapLandWater.begin(), perlinMapLandWater.end(), perlinMapLandWater.begin(),
        std::bind(std::plus<float>(), std::placeholders::_1, -waterLevelOffset));

    std::transform(perlinMapLandWater.begin(), perlinMapLandWater.end(), perlinMapLandWater.begin(),
                [](float val) {return std::clamp(val, 0.0f, 1.0f);}
                );

    return perlinMapLandWater;
}