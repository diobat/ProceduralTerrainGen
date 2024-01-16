#include "structures/LF_landDetail.hpp"
#include "map/MapGenerator.hpp"

#include <algorithm>
#include <functional>

std::vector<float> genLandLFDetail(unsigned int seed, unsigned int size, float scale) {

    std::vector<float> perlinMapSlopeNormalized = generatePerlinMap(seed+50, size, size, 10, 0.009f * scale, 1.5f, 1.0f, 0.5f);

    std::transform(perlinMapSlopeNormalized.begin(), perlinMapSlopeNormalized.end(), perlinMapSlopeNormalized.begin(),
                   [](float val) { 
                    float value = 1.0f - std::abs(val);
                    return value * value;
                    }
    );

    // Re-normalize after multiplying with land/water
    float maxSlope = *std::max_element(perlinMapSlopeNormalized.begin(), perlinMapSlopeNormalized.end());

    std::transform(perlinMapSlopeNormalized.begin(), perlinMapSlopeNormalized.end(), perlinMapSlopeNormalized.begin(),
        std::bind(std::divides<float>(), std::placeholders::_1, maxSlope));

    std::vector<float> perlinMapSlope(perlinMapSlopeNormalized);

    unsigned int exponentSlope = 1;
    // To accentuate slopes, we square the map and then normalize it

    for (unsigned int i(0); i < exponentSlope; ++i)
    {
        std::transform(perlinMapSlopeNormalized.begin(), perlinMapSlopeNormalized.end(), perlinMapSlopeNormalized.begin(),
                    perlinMapSlopeNormalized.begin(), std::multiplies<float>());

        float maxSlope2 = *std::max_element(perlinMapSlopeNormalized.begin(), perlinMapSlopeNormalized.end());

        std::transform(perlinMapSlopeNormalized.begin(), perlinMapSlopeNormalized.end(), perlinMapSlopeNormalized.begin(),
            std::bind(std::divides<float>(), std::placeholders::_1, maxSlope2));
    }

    return perlinMapSlopeNormalized;

}