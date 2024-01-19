// Playground for FluxLumina
#include <FluxLumina.hpp>

#include <vector>
#include <string>
#include <array>
#include <random>
#include <numeric> 

#include "structures/core.hpp"                 // Planet
#include "geometry/chunkManager.hpp"           // chunkManager
#include "model/modelManager.hpp"              // modelManager



int main(int argc, char** argv)
{
    FluxLumina engine(E_RenderStrategy::ForwardShading);

    // Camera setup
    engine.create_Camera(70.0f, 10.0f, 0.001f); 

    // Skybox setup
    engine.create_Skybox({
        "res/models/skybox/right.jpg",
        "res/models/skybox/left.jpg",
        "res/models/skybox/top.jpg",
        "res/models/skybox/bottom.jpg",
        "res/models/skybox/front.jpg",
        "res/models/skybox/back.jpg"
        });

    // Generate all necessary Perlin noise maps
    unsigned int N = 700;       // Size of the side of the map
    float global_scale = 1.0f;

    chunkManager chkMgr(N, 0.7f, global_scale, 0);
    modelManager mdlMgr(&engine);

    int gridSize = 3;   

    for(int x(-1); x < gridSize; ++x)
    {
        for(int y(-1); y < gridSize; ++y)
        {
            chkMgr.generateChunk(x, y);
            mdlMgr.generateModel({x, y}, N, chkMgr.getChunkData(x, y));
        }
    }

    // Point Lights
    auto light_A = engine.create_LightSource(1);
    engine.setColor(light_A, {0.4f, 0.4f, 0.4f});
    engine.setPosition(light_A, {(N*gridSize/2.0f), 80.0f, (N*gridSize/2.0f)});
    engine.setAttenuationFactors(light_A, {1.0f, 0.000000f, 0.000000f});


    // Begin main loop
    engine.update();
}


