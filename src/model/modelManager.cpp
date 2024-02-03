#include "model/modelManager.hpp"
#include "geometry/chunkManager.hpp"
#include <FluxLumina.hpp>

#include <algorithm>

namespace
{
    std::array<float, 3> normalizeVector(const std::array<float, 3>& a) 
    {
        std::array<float, 3> result(a);
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

    std::array<float, 3> crossProduct(const std::array<float, 3>& a, const std::array<float, 3>& b) 
    {
        std::array<float, 3> result;
        result[0] = a[1] * b[2] - a[2] * b[1]; // c_x
        result[1] = a[2] * b[0] - a[0] * b[2]; // c_y
        result[2] = a[0] * b[1] - a[1] * b[0]; // c_z

        result = normalizeVector(result);

        return result;
    }

    std::array<std::array<float,3> , 20> colorTable;
    unsigned int steps = colorTable.size();

    float interpolate(float a, float b, float t)
    {
        return a + (b - a) * t;
    }

    void populateColorTable(std::array<float,3> colorA, std::array<float,3> colorB)
    {

        for (unsigned int i(0); i < steps; ++i)
        {
            float t = static_cast<float>(i) / static_cast<float>(steps);

            colorTable[i][0] = interpolate(colorA[0], colorB[0], t);
            colorTable[i][1] = interpolate(colorA[1], colorB[1], t);
            colorTable[i][2] = interpolate(colorA[2], colorB[2], t);
        }
    }

    std::array<float,3> getColor(float value)
    {
        value = std::clamp(value, 0.0f, 1.0f);
        unsigned int index = static_cast<unsigned int>(value * static_cast<float>(steps));

        return colorTable[index];
    }
}

modelManager::modelManager(FluxLumina* engine, chunkManager* chunkMgr) :
    _engine(engine),
    _chunkMgr(chunkMgr)
{
    ;
}


void modelManager::generateModel(const std::array<int,2>& worldTileCoordinates, unsigned int sideLength, const std::vector<float>& data, float global_scale)
{
    unsigned int N = sideLength;

   // Create an N by N grid of vertices in the XZ plane
    std::vector<std::array<float,3>> vertices;

    float hScale = global_scale * 0.05f;

    for(int i(0); i < N; ++i)
    {
        for(int j(0); j < N; ++j)
        {
            float y = data[i*N+j];
            vertices.push_back({static_cast<float>(i)* hScale, y * hScale, static_cast<float>(j)* hScale});
        }
    }

    std::vector<unsigned int> indices;

    for(int i(0); i < N - 1; ++i)
    {
        for(int j(0); j < N - 1; ++j)
        {
            // // If all the vertices heights are 0, skip this square
            // if (vertices[i * N + j][1] == 0.0f &&
            //     vertices[i * N + j + 1][1] == 0.0f &&
            //     vertices[(i + 1) * N + j][1] == 0.0f &&
            //     vertices[(i + 1) * N + j + 1][1] == 0.0f)
            // {
            //     continue;
            // }

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

    std::vector<std::array<float,3>> colors = calculateColor(worldTileCoordinates, sideLength, hScale, normals);


    // Generate a model for the water
    std::vector<std::array<float,3>> waterVertices;
    std::vector<std::array<float,3>> waterNormals;
    std::vector<unsigned int> waterIndices;
    std::vector<std::array<float,3>> waterColors;

    float sideLengthFloat = static_cast<float>(sideLength);

    waterVertices.push_back({0.0f * hScale * sideLengthFloat, 0.0f, 0.0f* hScale * sideLengthFloat});
    waterVertices.push_back({0.0f * hScale * sideLengthFloat, 0.0f, 1.0f* hScale * sideLengthFloat});
    waterVertices.push_back({1.0f * hScale * sideLengthFloat, 0.0f, 0.0f* hScale * sideLengthFloat});
    waterVertices.push_back({1.0f * hScale * sideLengthFloat, 0.0f, 1.0f* hScale * sideLengthFloat});

    waterIndices.push_back(0);
    waterIndices.push_back(1);
    waterIndices.push_back(2);

    waterIndices.push_back(1);
    waterIndices.push_back(3);
    waterIndices.push_back(2);


    waterNormals.push_back({0.0f, 1.0f, 0.0f});
    waterNormals.push_back({0.0f, 1.0f, 0.0f});
    waterNormals.push_back({0.0f, 1.0f, 0.0f});
    waterNormals.push_back({0.0f, 1.0f, 0.0f});

    waterColors.push_back({0.0f, 0.11f, 0.38f});
    waterColors.push_back({0.0f, 0.11f, 0.38f});
    waterColors.push_back({0.0f, 0.11f, 0.38f});
    waterColors.push_back({0.0f, 0.11f, 0.38f});

    // Calculate the world position of the model
    int sideLengthInt = static_cast<int>(sideLength);
    std::array<float, 3> modelPosition = {
        static_cast<float>(worldTileCoordinates[0] * (sideLengthInt-1) - (sideLengthInt/2)) * hScale,
        0.0f, 
        static_cast<float>(worldTileCoordinates[1] * (sideLengthInt-1) - (sideLengthInt/2)) * hScale
    };

    std::array<float, 3> waterModelPosition = {
        static_cast<float>(worldTileCoordinates[0] * (sideLengthInt-1) - (sideLengthInt/2)) * hScale,
        -10.0f, 
        static_cast<float>(worldTileCoordinates[1] * (sideLengthInt-1) - (sideLengthInt/2)) * hScale
    };

    // boost::uuids::uuid waterModelID = _engine->create_Model(waterVertices, waterNormals, waterIndices, waterColors, "water");
    // _engine->setPosition(waterModelID, waterModelPosition);

    if(indices.size() != 0)
    {    
        boost::uuids::uuid modelID = _engine->create_Model(vertices, normals, indices, colors, "map");
        _engine->setPosition(modelID, modelPosition);
        _generatedChunkModels[worldTileCoordinates] = modelID;
    }
    else
    {
        // _generatedChunkModels[worldTileCoordinates] = waterModelID;
    }
}

std::vector<std::array<float, 3>> modelManager::calculateColor(const std::array<int, 2>& worldTileCoordinates, unsigned int sideLength, float hScale, const std::vector<std::array<float, 3>> normals) const
{

    std::array<float,3> color_green = {0.41f, 0.8f, 0.37f};
    std::array<float,3> color_dark_green = {0.11f, 0.35f, 0.07f};
    std::array<float,3> color_very_dark_green = {0.055f, 0.175f, 0.035f};
    std::array<float,3> color_gray = {0.43f, 0.45f, 0.45f};
    std::array<float,3> color_white = {1.0f, 1.0f, 1.0f};
    std::array<float,3> color_blue = {0.0f, 0.11f, 0.38f};
    std::array<float,3> color_brown = {0.30f, 0.08f, 0.03f};
    populateColorTable(color_green, color_dark_green);

    std::vector<std::array<float, 3>> colors;

    std::unique_ptr<Planet>& chunk = _chunkMgr->getChunk(worldTileCoordinates[0], worldTileCoordinates[1]);

    std::vector<float> LandWater = chunk->getLandWater();
    std::vector<float> Slope = chunk->getSlope();
    std::vector<float> Mountains = chunk->getMountains();

    
    for(unsigned int i(0); i < sideLength * sideLength; ++i)
    {
        if(normals[i][1] > 0.98f)
        {
            colors.push_back(color_very_dark_green);
        }
        else if (normals[i][1] > 0.9f)
        {
            colors.push_back(color_dark_green);
        }
        else
        {
            colors.push_back(color_gray);
        }

        // if (LandWater[i] <= 0.0f)
        // {
        //     colors.push_back(color_blue);
        // }
        // else 
        // if (Mountains[i] > 0.75f && Slope[i] > 0.5f)
        // {
        //     colors.push_back(color_white);
        // }
        // else if (Mountains[i] > 0.35f)
        // {
        //     if (Slope[i] > 0.5f)
        //     {
        //         colors.push_back(color_gray);
        //     }
        //     else
        //     {
        //         colors.push_back(color_dark_green);
        //     }
        // }
        // else
        // {
        //     colors.push_back(getColor( std::min(LandWater[i], 0.99f)));
        // }
    }
    
    return colors;
}

bool modelManager::modelExists(const std::array<int, 2>& worldTileCoordinates) const
{
    return _generatedChunkModels.find(worldTileCoordinates) != _generatedChunkModels.end();
}

boost::uuids::uuid modelManager::getModel(const std::array<int, 2>& worldTileCoordinates) const
{
    return _generatedChunkModels.at(worldTileCoordinates);
}

const std::unordered_map<std::array<int, 2>, boost::uuids::uuid, ArrayHash>& modelManager::allModels() const
{
    return _generatedChunkModels;
}