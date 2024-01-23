#pragma once

#include <array>
#include <unordered_map>
#include <vector>
#include <boost/uuid/uuid.hpp>

#include "util/hashFunc.hpp"

class FluxLumina;
class chunkManager;

class modelManager
{
public:
    modelManager(FluxLumina* engine);

    void generateModel(const std::array<int, 2>& worldTileCoordinates , unsigned int sideLength, const std::vector<float>& data, float zoom = 1.0f);

    bool modelExists(const std::array<int, 2>& worldTileCoordinates) const;
    boost::uuids::uuid getModel(const std::array<int, 2>& worldTileCoordinates) const;
    const std::unordered_map<std::array<int, 2>, boost::uuids::uuid, ArrayHash>& allModels() const;

private:
    std::unordered_map<std::array<int, 2>, boost::uuids::uuid, ArrayHash> _generatedChunkModels;

    FluxLumina* _engine;
};