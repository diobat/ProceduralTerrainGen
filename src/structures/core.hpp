#pragma once

#include <vector>
#include <array>

class FluxLumina;

class Planet 
{
public:
    Planet(unsigned int sideSize,  const std::array<int, 2>& offsetCoord, float waterPercentage, float globalscale = 1.0f, unsigned int seed = 0);

    void nuke();

    const std::vector<float>& bake();                   // Bake the planet and return the height values
    const std::vector<float>& getTerrain() const;       // Return the height values of the planet without baking

    void bindCallbacks(FluxLumina& engine);
    void move(float x, float y);
    void zoom(float factor);
    const std::array<int, 2>& getChunkPosition() const;

    std::array<unsigned int, 2> getDimensions() const;

    // HydroErosion
    void hydroErosionStep();

    // Terrain gain factors
    void setLandWaterWeight(float weight);
    void setSlopeWeight(float weight);
    void setMountainsWeight(float weight);
    void setRuggedWeight(float weight);

    // Terrain getters
    const std::vector<float>& getLandWater() const { return _mapLandWater; };
    const std::vector<float>& getSlope() const { return _mapSlopeBase; };
    const std::vector<float>& getMountains() const { return _mapMountainsBase; };

private:

    std::vector<float> regenerate();

    // Planetary component layers
    // Landmasses
    void genPlanetLandmasses(int seedOffset);
    std::vector<float> _mapLandWater;
    std::vector<float> _mapLandWaterFilter;
    float _LandWater_Weight;
    // LF Slope
    void genPlanetLFSlope(int seedOffset);
    std::vector<float> _mapSlopeBase;
    std::vector<float> _mapSlope;
    float _SlopeBase_Weight;
    // Moutains
    void genPlanetMountains(int seedOffset);
    std::vector<float> _mapMountainsBase;
    std::vector<float> _mapMountains;
    float _Mountains_Weight;
    // Roughness
    void genPlanetRugged(int seedOffset);
    std::vector<float> _mapRugged;
    float _Rugged_Weight;

    // Resulting Output
    std::vector<float> _planet;

    // Auxiliary components
    unsigned int _seed;
    unsigned int _mapSideSize;
    std::array<int, 2> _worldGridPosition;

    float _waterPercentage;
    float _globalscale;
    unsigned int _windowSideSize;
};