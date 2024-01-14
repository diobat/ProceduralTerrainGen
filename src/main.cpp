// Playground for FluxLumina
#include <FluxLumina.hpp>
#include <GraphicalEngine.hpp>

#include <vector>
#include <string>
#include <array>

#include "map/MapGenerator.hpp"

void scene4(FluxLumina& engine);

int main(int argc, char** argv)
{
    FluxLumina engine(E_RenderStrategy::ForwardShading);

    scene4(engine);

    // Begin main loop
    engine.update();
}


void scene4(FluxLumina& engine)
{
    // Camera setup
    engine.create_Camera();

    // Disable shadows


    // Skybox setup
    engine.create_Skybox({
        "res/models/skybox/right.jpg",
        "res/models/skybox/left.jpg",
        "res/models/skybox/top.jpg",
        "res/models/skybox/bottom.jpg",
        "res/models/skybox/front.jpg",
        "res/models/skybox/back.jpg"
        });

    // Generate a Perlin noise map

    int N = 1000;

    std::vector<float> perlinMap = generatePerlinMap(500, N, N, 3, 0.02f, 2.0f, 0.9f);

    // Create an N by N grid of vertices in the XZ plane

    std::vector<std::array<float,3>> vertices;

    for(int i(0); i < N; ++i)
    {
        for(int j(0); j < N; ++j)
        {
            vertices.push_back({static_cast<float>(i), perlinMap[i*N+j]*20.0f, static_cast<float>(j) * 1.0f});
        }
    }

    std::vector<unsigned int> indices;

    for(int i(0); i < N - 1; ++i)
    {
        for(int j(0); j < N - 1; ++j)
        {
            indices.push_back(i * N + j);
            indices.push_back(i * N + j + 1);
            indices.push_back((i + 1) * N + j);

            indices.push_back((i + 1) * N + j);
            indices.push_back(i * N + j + 1);
            indices.push_back((i + 1) * N + j + 1);
        }
    }

    engine.create_Model(vertices, indices, "Basic");

    // Point Lights
    auto light_A = engine.create_LightSource(1);
    engine.setColor(light_A, {0.4f, 0.4f, 0.4f});
    engine.setPosition(light_A, {N/2.0f, 50.0f, N/2.0f});
    engine.setAttenuationFactors(light_A, {1.0f, 0.000000f, 0.000000f});

}