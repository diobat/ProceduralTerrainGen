#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include "structures/core.hpp"    
#include "util/hashFunc.hpp"

#include "model/modelManager.hpp"     

class FluxLumina;
class chunkManager
{
public:
    chunkManager(FluxLumina* engine, unsigned int sideSize, float waterPercentage, float globalscale = 1.0f, unsigned int seed = 0);
    ~chunkManager();

    std::unique_ptr<Planet>& getChunk(int x, int y);
    void generateChunk(int x, int y);

    const std::vector<float>& getChunkData(int x, int y);

    void positionCallback();
    void hydroErosionCallback();

    unsigned int getChunkSize() const;
    float getGlobalScale() const;
    float getWaterPercentage() const;
    float getSeed() const;

private:
    std::unordered_map<std::array<int, 2>, std::unique_ptr<Planet>, ArrayHash> _generatedChunks;

    FluxLumina* _engine;
    modelManager _mdlMgr; 

    unsigned int _chunkSize;
    float _globalScale;
    float _waterPercentage;
    void randomizeSeed();
    unsigned int _seed;
};