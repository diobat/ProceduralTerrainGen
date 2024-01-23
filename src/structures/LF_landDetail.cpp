#include "structures/LF_landDetail.hpp"
#include "map/MapGenerator.hpp"

#include <algorithm>
#include <functional>

namespace
{
    float divisor = 0.0f;

    void findPercentile(std::vector<float> perlinMap, float percentile)
    {
        std::sort(perlinMap.begin(), perlinMap.end());

        unsigned int index = perlinMap.size() * percentile;

        divisor = perlinMap[index];
    }
}

std::vector<float> genLandLFDetail(unsigned int seed, unsigned int size, std::array<int, 2> xyCenter, float scale) {

    std::vector<float> perlinMapSlopeNormalized = generatePerlinMap(seed, size, size, xyCenter[0], xyCenter[1], 4, 0.004f / scale, 1.5f, 1.0f, 0.85f);

    std::transform(perlinMapSlopeNormalized.begin(), perlinMapSlopeNormalized.end(), perlinMapSlopeNormalized.begin(),
                   [](float val) { 
                    float value = 1.0f - std::abs(val);
                    return value * value;
                    }
    );

    // // find max value of slope base
    // auto max = std::max_element(perlinMapSlopeNormalized.begin(), perlinMapSlopeNormalized.end());

    // // divide every member by twice that value
    // std::transform(perlinMapSlopeNormalized.begin(), perlinMapSlopeNormalized.end(), perlinMapSlopeNormalized.begin(),
    //                std::bind(std::divides<float>(), std::placeholders::_1, *max * 2.0f));

    // To accentuate slopes, we square the map and then normalize it

    for (unsigned int i(0); i < 1; ++i)
    {
        std::transform(perlinMapSlopeNormalized.begin(), perlinMapSlopeNormalized.end(), perlinMapSlopeNormalized.begin(),
                    perlinMapSlopeNormalized.begin(), std::multiplies<float>());
    }

    return perlinMapSlopeNormalized;

}