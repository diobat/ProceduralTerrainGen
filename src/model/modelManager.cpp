#include "model/modelManager.hpp"
#include <FluxLumina.hpp>

#include <algorithm>

namespace
{
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



modelManager::modelManager(FluxLumina* engine) :
    _engine(engine)
{
    ;
}


void modelManager::generateModel(const std::array<int,2>& worldTileCoordinates, unsigned int sideLength, const std::vector<float>& data)
{
    unsigned int N = sideLength;

   // Create an N by N grid of vertices in the XZ plane
    std::vector<std::array<float,3>> vertices;

    float hScale = 1.0f;

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

    std::array<float,3> color_green = {0.31f, 0.65f, 0.27f};
    std::array<float,3> color_dark_green = {0.11f, 0.35f, 0.07f};
    std::array<float,3> color_gray = {0.43f, 0.45f, 0.45f};
    std::array<float,3> color_white = {0.95f, 0.95f, 0.95f};
    std::array<float,3> color_blue = {0.0f, 0.18f, 0.30f};


    populateColorTable(color_green, color_dark_green);

    for (std::size_t i(0); i < vertices.size(); ++i)
    {
        
        float y = vertices[i][1];

        if (y <= 0.0f * hScale)
        {
            colors.push_back(color_blue);
        }
        else if (y < 0.5f * hScale)
        {
            colors.push_back(color_green);
        }
        else if (y < 7.0f * hScale)
        {
            colors.push_back(getColor((y) / (7.0f * hScale)));
        }
        else if (y < 8.0f * hScale)
        {
            colors.push_back(color_gray);
        }
        else
        {
            colors.push_back(color_white);
        }
    }
    
    boost::uuids::uuid modelID = _engine->create_Model(vertices, indices, colors, "Basic");

    _engine->setPosition(modelID, {
            static_cast<float>(worldTileCoordinates[0] * sideLength) * hScale,
            0.0f, 
            static_cast<float>(worldTileCoordinates[1] * sideLength) * hScale
    });

    _generatedChunks[worldTileCoordinates] = modelID;

}