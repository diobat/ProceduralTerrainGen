#include "noise/PerlinNoise.hpp"

#include <FastNoise/FastNoise.h>

std::vector<float> generatePerlinNoise(unsigned int seed, unsigned int width, unsigned int height, int xCenter, int yCenter, float frequency, float amplitude)
{
    std::vector<float> perlinMap(width * height);

    auto nPerlin = FastNoise::New<FastNoise::Perlin>();
    
    nPerlin->GenUniformGrid2D(perlinMap.data(), yCenter * width , xCenter * height , width, height, frequency, seed);

    std::transform(perlinMap.begin(), perlinMap.end(), perlinMap.begin(),
                std::bind(std::multiplies<float>(), std::placeholders::_1, amplitude));

    return perlinMap;
}

std::vector<float> generateTilePerlinNoise(unsigned int seed, unsigned int width, unsigned int height, float frequency, float amplitude)
{
    std::vector<float> perlinMap(width * height);

    auto nPerlin = FastNoise::New<FastNoise::Perlin>();

    nPerlin->GenTileable2D(perlinMap.data(), width, height, frequency, seed);

    std::transform(perlinMap.begin(), perlinMap.end(), perlinMap.begin(),
                std::bind(std::multiplies<float>(), std::placeholders::_1, amplitude));

    return perlinMap;
}