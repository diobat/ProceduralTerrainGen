#pragma once

#include <array>
#include <memory>
#include "structures/core.hpp"

class chunkManager;

class chunkData
{
public:
    chunkData(chunkManager* manager, std::array<int, 2> position);

    std::vector<float> generateChunkData();
    const std::vector<float>& getChunkData() const;

    bool isLoaded() const;
    void setLoaded(bool loaded);


private:
    std::array<int, 2> _worldGridPosition;
    Planet _planet;
    bool _isLoaded;
    chunkManager* _manager;
};