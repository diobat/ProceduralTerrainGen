#include "geometry/chunkManager.hpp"

#include <random>

chunkManager::chunkManager(unsigned int sideSize, float waterPercentage, float globalscale, unsigned int seed ) :
    _chunkSize(sideSize),
    _globalScale(globalscale),
    _waterPercentage(waterPercentage),
    _seed(seed)
{
    if (_seed == 0)
    {
        randomizeSeed();
    }
}

chunkManager::~chunkManager()
{
    ;
}

void chunkManager::generateChunk(int x, int y)
{
    chunkData newChunk(this, {x, y});
    _generatedChunks[{x, y}] = std::make_unique<chunkData>(newChunk);
    _generatedChunks[{x, y}]->generateChunkData();
}

std::vector<float> chunkManager::getChunkData(int x, int y)
{
    return _generatedChunks[{x, y}]->getChunkData();
}


unsigned int chunkManager::getChunkSize() const
{
    return _chunkSize;
}

float chunkManager::getGlobalScale() const
{
    return _globalScale;
}


float chunkManager::getWaterPercentage() const
{
    return _waterPercentage;
}

float chunkManager::getSeed() const
{
    return _seed;
}

void chunkManager::randomizeSeed()
{
    std::random_device rd;
    std::mt19937 rng(rd()); // Mersenne Twister random number generator
    std::uniform_int_distribution<unsigned int> dist(0, std::numeric_limits<unsigned int>::max());
    _seed = dist(rng);
}