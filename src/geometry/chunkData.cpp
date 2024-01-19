#include "geometry/chunkData.hpp"
#include "geometry/chunkManager.hpp"
#include "structures/core.hpp"    


chunkData::chunkData(chunkManager* manager, std::array<int, 2> position) :
    _worldGridPosition(position),
    _planet(Planet(manager->getChunkSize(), position, manager->getWaterPercentage(), manager->getGlobalScale(), manager->getSeed())),
    _isLoaded(false),
    _manager(manager)
{
    ;
}

std::vector<float> chunkData::generateChunkData()
{
    return _planet.bake();
}

const std::vector<float>& chunkData::getChunkData() const
{
    return _planet.getTerrain();
}

bool chunkData::isLoaded() const
{
    return _isLoaded;
}