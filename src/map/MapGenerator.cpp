#include "MapGenerator.hpp"

#include <algorithm>
#include <functional>

#include "noise/PerlinNoise.hpp"

std::vector<float> generatePerlinMap(unsigned int seed, unsigned int width, unsigned int heigh, unsigned int octaves, float initialFrequency, float lacunarity, float initialAmplitude, float persistence)
{
    std::vector<float> perlinMap(width * heigh);

    float amplitude = initialAmplitude;
    float frequency = initialFrequency;

    for (unsigned int i(0); i < octaves; ++i)
    {
        std::vector<float> currentOctave = generatePerlinNoise(seed + i, width, heigh, frequency, amplitude);

        std::transform(perlinMap.begin(), perlinMap.end(), currentOctave.begin(),
                    perlinMap.begin(), std::plus<float>());

        amplitude *= persistence;
        frequency *= lacunarity;
    }
    return perlinMap;
}

std::vector<float> generateTilePerlinMap(unsigned int seed, unsigned int width, unsigned int heigh, unsigned int octaves, float initialFrequency, float lacunarity, float initialAmplitude, float persistence)
{
    std::vector<float> perlinMap(width * heigh);

    float amplitude = initialAmplitude;
    float frequency = initialFrequency;

    for (unsigned int i(0); i < octaves; ++i)
    {
        std::vector<float> currentOctave = generateTilePerlinNoise(seed + i, width, heigh, frequency, amplitude);

        std::transform(perlinMap.begin(), perlinMap.end(), currentOctave.begin(),
                    perlinMap.begin(), std::plus<float>());

        amplitude *= persistence;
        frequency *= lacunarity;
    }
    return perlinMap;
}