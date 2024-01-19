#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include "geometry/chunkData.hpp"
#include "util/hashFunc.hpp"


class chunkManager
{
public:
    chunkManager(unsigned int sideSize, float waterPercentage, float globalscale = 1.0f, unsigned int seed = 0);
    ~chunkManager();

    void generateChunk(int x, int y);

    std::vector<float> getChunkData(int x, int y);

    unsigned int getChunkSize() const;
    float getGlobalScale() const;
    float getWaterPercentage() const;
    float getSeed() const;

private:
    std::unordered_map<std::array<int, 2>, std::unique_ptr<chunkData>, ArrayHash> _generatedChunks;

    unsigned int _chunkSize;
    float _globalScale;
    float _waterPercentage;
    void randomizeSeed();
    unsigned int _seed;
};