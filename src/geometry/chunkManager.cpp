#include "geometry/chunkManager.hpp"
#include "structures/core.hpp"    
#include <FluxLumina.hpp>

#include <random>
#include <cmath>
#include <thread>


chunkManager::chunkManager(FluxLumina* engine, unsigned int sideSize, float waterPercentage, float globalscale, unsigned int seed ) :
    _engine(engine),
    _chunkSize(sideSize),
    _globalScale(globalscale),
    _waterPercentage(waterPercentage),
    _seed(seed),
    _mdlMgr(engine)
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
    _generatedChunks[{x, y}] = std::make_unique<Planet>(_chunkSize, std::array<int, 2>({x, y}), _waterPercentage, _globalScale, _seed);
    _generatedChunks[{x, y}]->bake();
    _mdlMgr.generateModel({x, y}, _chunkSize, getChunkData(x, y), _globalScale);
}

std::vector<float> chunkManager::getChunkData(int x, int y)
{
    return _generatedChunks[{x, y}]->getTerrain();   
}

void chunkManager::positionCallback()
{
    std::array<float, 3> position = _engine->getCameraPosition();

    std::array<int, 2> chunkPosition = {
        static_cast<int>(std::round((position[0]/_globalScale) / _chunkSize)),
        static_cast<int>(std::round((position[2]/_globalScale) / _chunkSize))
        };


    for (const auto& model : _mdlMgr.allModels())
    {
        _engine->setEnabled(model.second, false);
    }

    int chunkRadius = 3;

    for (int x = chunkPosition[0] - chunkRadius; x <= chunkPosition[0] + chunkRadius; ++x)
    {
        for (int y = chunkPosition[1] - chunkRadius; y <= chunkPosition[1] + chunkRadius; ++y)
        {
            if (_generatedChunks.find({x, y}) == _generatedChunks.end())
            {
                _generatedChunks[{x, y}] = nullptr;
                std::thread worker(&chunkManager::generateChunk, this, x, y);
                worker.detach();
            }
            else if(_mdlMgr.modelExists({x, y}))
            {
                _engine->setEnabled(_mdlMgr.getModel({x, y}), true);
            }
        }
    }
};

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