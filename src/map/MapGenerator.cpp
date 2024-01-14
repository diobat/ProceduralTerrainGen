#include "MapGenerator.hpp"

#include <algorithm>
#include <functional>

#include "noise/PerlinNoise.hpp"

std::vector<float> generatePerlinMap(unsigned int seed, unsigned int width, unsigned int heigh, unsigned int octaves, float initialFrequency, float lacunarity, float persistence)
{
    std::vector<float> perlinMap(width * heigh);

    float amplitude = 1.0f;
    float frequency = initialFrequency;

    for (unsigned int i(0); i < octaves; ++i)
    {
        std::vector<float> currentOctave = generatePerlinNoise(seed + i, width, heigh, frequency, amplitude);

        std::transform(perlinMap.begin(), perlinMap.end(), currentOctave.begin(),
                    perlinMap.begin(), std::plus<float>());

        amplitude *= persistence;
        frequency *= lacunarity;
    }


    // To accentuate slopes, we square the map and then normalize it
    std::transform(perlinMap.begin(), perlinMap.end(), perlinMap.begin(),
                perlinMap.begin(), std::multiplies<float>());

    float max = *std::max_element(perlinMap.begin(), perlinMap.end());

    std::transform(perlinMap.begin(), perlinMap.end(), perlinMap.begin(),
                std::bind(std::divides<float>(), std::placeholders::_1, max));



    return perlinMap;
}