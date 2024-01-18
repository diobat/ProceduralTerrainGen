#pragma once

#include <vector>

class FluxLumina;

class Planet 
{
public:
    Planet(unsigned int sideSize, float waterPercentage, float globalscale = 1.0f, unsigned int seed = 0);

    void initialize();
    void nuke();

    const std::vector<float>& bake();                   // Bake the planet and return the height values
    const std::vector<float>& getTerrain() const;       // Return the height values of the planet without baking

    void bindCallbacks(FluxLumina& engine);
    void move(float x, float y);
    void zoom(float factor);
    std::array<float, 2> getOffset() const;

    std::array<unsigned int, 2> getDimensions() const;

    // Terrain gain factors
    void setLandWaterWeight(float weight);
    void setSlopeWeight(float weight);
    void setMountainsWeight(float weight);
    void setRuggedWeight(float weight);

private:

    std::vector<float> regenerate();

    // Planetary component layers
    // Landmasses
    void genPlanetLandmasses(int seedOffset);
    std::vector<float> _mapLandWater;
    std::vector<float> _mapLandWaterFilter;
    float _LandWater_Weight = 1.0f;
    // LF Slope
    void genPlanetLFSlope(int seedOffset);
    std::vector<float> _mapSlopeBase;
    std::vector<float> _mapSlope;
    float _SlopeBase_Weight = 1.0f;
    // Moutains
    void genPlanetMountains(int seedOffset);
    std::vector<float> _mapMountainsBase;
    std::vector<float> _mapMountains;
    float _Mountains_Weight = 1.0f;
    // Roughness
    void genPlanetRugged(int seedOffset);
    std::vector<float> _mapRugged;
    float _Rugged_Weight = 1.0f;

    // Resulting Output
    std::vector<float> _planet;

    // Auxiliary components
    unsigned int _seed;
    unsigned int _mapSideSize;
    float _waterPercentage;
    float _globalscale;
    float _hOffset; 
    float _vOffset;
    unsigned int _windowSideSize;
};