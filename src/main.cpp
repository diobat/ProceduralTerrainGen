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
    FluxLumina engine(E_RenderStrategy::ForwardShading, "res/shaders/");

    // Skybox setup
    engine.create_Skybox({
        "res/models/skybox/right.jpg",
        "res/models/skybox/left.jpg",
        "res/models/skybox/top.jpg",
        "res/models/skybox/bottom.jpg",
        "res/models/skybox/front.jpg",
        "res/models/skybox/back.jpg"
        });


    // Chunk size and scale
    unsigned int N = 700;       // Size of the side of the map
    float zoom_level = 100.0f;

    // Camera setup
    engine.create_Camera(70.0f, zoom_level / 6.0f, 0.001f); 
    engine.setCameraPosition({0.0f, 10.0f * zoom_level, 0.0f});

    // Generate all necessary Perlin noise maps
    chunkManager chkMgr(&engine, N, 0.55f, zoom_level, 0);

    bool GRT = false;   // Generate Real Time

    if(!GRT)
    {
        // int gridSize = static_cast<int>(std::ceil(zoom_level/2.0f)); 
        int gridSize = 1; 

        for(int x(-gridSize/2); x <= gridSize/2; ++x)
        {
            for(int y(-gridSize/2); y <= gridSize/2; ++y)
            {
                chkMgr.generateChunk(x, y);
            }
        }
    }
    else
    {
        // Sometimes the first pass sets calibration values, which clashes with multithreading
        // So we generate the first chunk in the main thread
        chkMgr.generateChunk(0, 0);
        // Add callback function to main loop
        engine.addUpdateCallback(std::function<void()>(std::bind(&chunkManager::positionCallback, &chkMgr)));
    }

    // Point Lights
    auto light_A = engine.create_LightSource(1);
    engine.setColor(light_A, {0.4f, 0.4f, 0.4f});
    engine.setPosition(light_A, {0.0f, 80.0f * zoom_level, 0.0f});
    engine.setAttenuationFactors(light_A, {1.0f, 0.000000f, 0.000000f});

    // Begin main loop
    engine.update();
}