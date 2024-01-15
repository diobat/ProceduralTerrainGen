// Playground for FluxLumina
#include <FluxLumina.hpp>

#include <vector>
#include <string>
#include <array>
#include <algorithm>
#include <functional>
#include <random>

#include "map/MapGenerator.hpp"

void scene4(FluxLumina& engine);
std::array<float, 3> crossProduct(const std::array<float, 3>& a, const std::array<float, 3>& b);
std::array<float, 3> normalizeVector(const std::array<float, 3>& a);

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
    engine.create_Camera(70.0f, 1.0f, 0.001f);

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

    // Generate a seed

    std::random_device rd;
    std::mt19937 rng(rd()); // Mersenne Twister random number generator
    std::uniform_int_distribution<unsigned int> dist(0, std::numeric_limits<unsigned int>::max());
    unsigned int seed = dist(rng);

    // Generate a Perlin noise map
    int N = 1000;       // Size of the side of the map

    // Land/Water
    std::vector<float> perlinMapLandWater = generatePerlinMap(seed+3, N, N, 2, 0.001f, 1.2f, 1.0f, 0.8f);

    // std::transform(perlinMapLandWater.begin(), perlinMapLandWater.end(), perlinMapLandWater.begin(),
    //         std::bind(std::plus<float>(), std::placeholders::_1, 1.0f));

    // std::transform(perlinMapLandWater.begin(), perlinMapLandWater.end(), perlinMapLandWater.begin(),
    //         std::bind(std::divides<float>(), std::placeholders::_1, 2.0f));

    float landmass_percentage = 1.0f;

    std::transform(perlinMapLandWater.begin(), perlinMapLandWater.end(), perlinMapLandWater.begin(),
            std::bind(std::plus<float>(), std::placeholders::_1, landmass_percentage));

    // By repeatedly raising to the power of two and then normalizing,
    //we can create a convincing separationg between land and sea
    unsigned int exponentWater = 1;
    for (unsigned int i(0); i < exponentWater; ++i)
    {
        std::transform(perlinMapLandWater.begin(), perlinMapLandWater.end(), perlinMapLandWater.begin(),
                    perlinMapLandWater.begin(), std::multiplies<float>());
    }

    float maxW = *std::max_element(perlinMapLandWater.begin(), perlinMapLandWater.end());
    std::transform(perlinMapLandWater.begin(), perlinMapLandWater.end(), perlinMapLandWater.begin(),
        std::bind(std::divides<float>(), std::placeholders::_1, maxW));



    // Good low freq slope (exponent = one)
    std::vector<float> perlinMapSlopeNormalized = generatePerlinMap(seed, N, N, 2, 0.008f, 1.2f, 1.0f, 0.8f);

    // Mountains everywhere
    std::vector<float> perlinMapMountains = generatePerlinMap(seed+1, N, N, 3, 0.058f, 1.5f, 25.0f, 0.8f);

    std::transform(perlinMapSlopeNormalized.begin(), perlinMapSlopeNormalized.end(), perlinMapSlopeNormalized.begin(),
            std::bind(std::plus<float>(), std::placeholders::_1, 1.0f));

    std::transform(perlinMapSlopeNormalized.begin(), perlinMapSlopeNormalized.end(), perlinMapSlopeNormalized.begin(),
            std::bind(std::divides<float>(), std::placeholders::_1, 2.0f));

    std::transform(perlinMapSlopeNormalized.begin(), perlinMapSlopeNormalized.end(), perlinMapLandWater.begin(),
                perlinMapSlopeNormalized.begin(), std::multiplies<float>());

    std::vector<float> perlinMapSlope(perlinMapSlopeNormalized);


    unsigned int exponentSlope = 2;

    // To accentuate slopes, we square the map and then normalize it
    for (unsigned int i(0); i < exponentSlope; ++i)
    {
        float max = *std::max_element(perlinMapSlopeNormalized.begin(), perlinMapSlopeNormalized.end());

        std::transform(perlinMapSlopeNormalized.begin(), perlinMapSlopeNormalized.end(), perlinMapSlopeNormalized.begin(),
                    std::bind(std::divides<float>(), std::placeholders::_1, max));

        std::transform(perlinMapSlopeNormalized.begin(), perlinMapSlopeNormalized.end(), perlinMapSlopeNormalized.begin(),
                    perlinMapSlopeNormalized.begin(), std::multiplies<float>());

        std::transform(perlinMapSlopeNormalized.begin(), perlinMapSlopeNormalized.end(), perlinMapSlopeNormalized.begin(),
            std::bind(std::multiplies<float>(), std::placeholders::_1, max));
    }

    unsigned int exponentMountains = 1;

    // To accentuate slopes, we square the map and then normalize it
    for (unsigned int i(0); i < exponentMountains; ++i)
    {
        float max = *std::max_element(perlinMapMountains.begin(), perlinMapMountains.end());

        std::transform(perlinMapMountains.begin(), perlinMapMountains.end(), perlinMapMountains.begin(),
                    std::bind(std::divides<float>(), std::placeholders::_1, max));

        std::transform(perlinMapMountains.begin(), perlinMapMountains.end(), perlinMapMountains.begin(),
                    perlinMapMountains.begin(), std::multiplies<float>());

        std::transform(perlinMapMountains.begin(), perlinMapMountains.end(), perlinMapMountains.begin(),
            std::bind(std::multiplies<float>(), std::placeholders::_1, max));
    }

    std::vector<float> perlinMap(perlinMapSlopeNormalized.size());

    std::transform(perlinMapSlopeNormalized.begin(), perlinMapSlopeNormalized.end(), perlinMapMountains.begin(),
            perlinMap.begin(), std::multiplies<float>());


    // Weight the ruggedness    
    std::vector<float> perlinMapRugged = generatePerlinMap(seed+2, N, N, 16, 0.3f, 5.0f, 4.0f, 0.4f);

    std::vector<float> normalizedPerlinMap(perlinMap.size());

    float max = *std::max_element(perlinMap.begin(), perlinMap.end());

    std::transform(perlinMap.begin(), perlinMap.end(), normalizedPerlinMap.begin(),
                std::bind(std::divides<float>(), std::placeholders::_1, max));

    std::transform(normalizedPerlinMap.begin(), normalizedPerlinMap.end(), perlinMapRugged.begin(),
        perlinMapRugged.begin(), std::multiplies<float>());
    std::transform(normalizedPerlinMap.begin(), normalizedPerlinMap.end(), perlinMapRugged.begin(),
        perlinMapRugged.begin(), std::multiplies<float>());
    std::transform(normalizedPerlinMap.begin(), normalizedPerlinMap.end(), perlinMapRugged.begin(),
        perlinMapRugged.begin(), std::multiplies<float>());


    // re add the landmass/water base

    std::transform(perlinMapLandWater.begin(), perlinMapLandWater.end(), perlinMapLandWater.begin(),
            std::bind(std::multiplies<float>(), std::placeholders::_1, 30.0f));

            
    std::transform(perlinMap.begin(), perlinMap.end(), perlinMapLandWater.begin(),
                perlinMap.begin(), std::plus<float>());


    // re add the slope at the end

    std::transform(perlinMapSlope.begin(), perlinMapSlope.end(), perlinMapSlope.begin(),
        std::bind(std::multiplies<float>(), std::placeholders::_1, 30.0f));

    std::transform(perlinMapSlope.begin(), perlinMapSlope.end(), perlinMap.begin(),
                perlinMap.begin(), std::plus<float>());

    std::transform(perlinMap.begin(), perlinMap.end(), perlinMapRugged.begin(),
                perlinMap.begin(), std::plus<float>());

    std::transform(perlinMap.begin(), perlinMap.end(), perlinMap.begin(),
        std::bind(std::plus<float>(), std::placeholders::_1, -8.5f));


    // Create an N by N grid of vertices in the XZ plane
    std::vector<std::array<float,3>> vertices;

    for(int i(0); i < N; ++i)
    {
        for(int j(0); j < N; ++j)
        {
            float y = perlinMap[i*N+j];
            vertices.push_back({static_cast<float>(i), y , static_cast<float>(j)});
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

    std::vector<std::array<float, 3>> normals(vertices.size(), {0.0f, 0.0f, 0.0f});
    for (std::size_t i = 0; i < indices.size(); i += 3)
    {
        // Get the three vertices that make the face
        std::array<float, 3> v1 = vertices[indices[i]];
        std::array<float, 3> v2 = vertices[indices[i + 1]];
        std::array<float, 3> v3 = vertices[indices[i + 2]];

        // Calculate two vectors from the three vertices
        std::array<float, 3> v1v2 = {v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2]};
        std::array<float, 3> v1v3 = {v3[0] - v1[0], v3[1] - v1[1], v3[2] - v1[2]};

        // Calculate the normal of the face
        std::array<float, 3> normal = crossProduct(v1v2, v1v3);

        // Add the normal to the three vertices
        normals[indices[i]][0] += normal[0];
        normals[indices[i]][1] += normal[1];
        normals[indices[i]][2] += normal[2];

        normals[indices[i + 1]][0] += normal[0];
        normals[indices[i + 1]][1] += normal[1];
        normals[indices[i + 1]][2] += normal[2];

        normals[indices[i + 2]][0] += normal[0];
        normals[indices[i + 2]][1] += normal[1];
        normals[indices[i + 2]][2] += normal[2];
    }

    // Normalize all the sums of normals
    for (std::size_t i = 0; i < normals.size(); i++)
    {
        normals[i] = normalizeVector(normals[i]);
    }


    std::vector<std::array<float,3>> colors;

    std::array<float,3> color_green = {0.41f, 0.75f, 0.37f};
    std::array<float,3> color_brown = {0.21f, 0.06f, 0.02f};
    std::array<float,3> color_gray = {0.43f, 0.45f, 0.45f};
    std::array<float,3> color_white = {0.95f, 0.95f, 0.95f};
    std::array<float,3> color_blue = {0.0f, 0.18f, 0.30f};


    for (std::size_t i(0); i < vertices.size(); ++i)
    {
        
        float y = vertices[i][1];

        if (y < 1.0f)
        {
            colors.push_back(color_blue);
        }
        else if (y < 25.0f)
        {
            colors.push_back(color_green);
        }
        else if (y < 35.0f)
        {
            if(normals[i][1] < 0.5f)
                colors.push_back(color_green);
            else
                colors.push_back(color_brown);
        }
        else if (y < 40.0f)
        {
            colors.push_back(color_gray);
        }
        else
        {
            colors.push_back(color_white);
        }
    }


    engine.create_Model(vertices, indices, colors, "Basic");

    // Point Lights
    auto light_A = engine.create_LightSource(1);
    engine.setColor(light_A, {0.4f, 0.4f, 0.4f});
    engine.setPosition(light_A, {N/2.0f, 50.0f, N/2.0f});
    engine.setAttenuationFactors(light_A, {1.0f, 0.000000f, 0.000000f});

}

std::array<float, 3> crossProduct(const std::array<float, 3>& a, const std::array<float, 3>& b) 
{
    std::array<float, 3> result;
    result[0] = a[1] * b[2] - a[2] * b[1]; // c_x
    result[1] = a[2] * b[0] - a[0] * b[2]; // c_y
    result[2] = a[0] * b[1] - a[1] * b[0]; // c_z

    result = normalizeVector(result);

    return result;
}


std::array<float, 3> normalizeVector(const std::array<float, 3>& a) 
{
    std::array<float, 3> result;
    // Calculate the magnitude of the vector
    float magnitude = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);

    // Normalize the vector
    if (magnitude != 0) { // Check to avoid division by zero
        result[0] = a[0] / magnitude;
        result[1] = a[1] / magnitude;
        result[2] = a[2] / magnitude;
    }

    return result;
}