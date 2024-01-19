#include "core.hpp"

#include <random>
#include <functional>
#include <numeric> 
#include <array>
#include <GLFW/glfw3.h>
#include <FluxLumina.hpp>

#include <iostream>

#include "structures/landmass.hpp"
#include "structures/LF_landDetail.hpp"
#include "structures/mountains.hpp"
#include "structures/roughness.hpp"

Planet::Planet(unsigned int sideSize, const std::array<int, 2>& offsetCoords ,float waterPercentage, float globalscale, unsigned int seed) :
    _seed(seed),
    _mapSideSize(sideSize),
    _waterPercentage(waterPercentage),
    _globalscale(globalscale),
    _worldGridPosition(offsetCoords)
{
    initialize();

    _LandWater_Weight = 10.0f;
    _SlopeBase_Weight = 15.0f;
    _Mountains_Weight = 5.0f;
    _Rugged_Weight = 1.0f;
}

void Planet::initialize()
{
    if(_seed == 0)
    {
        std::random_device rd;
        std::mt19937 rng(rd()); // Mersenne Twister random number generator
        std::uniform_int_distribution<unsigned int> dist(0, std::numeric_limits<unsigned int>::max());
        unsigned int _seed = dist(rng);
    }
}

/////// INTERACTION ///////

void Planet::nuke()
{
    _mapLandWater.clear();
    _mapSlopeBase.clear();
    _mapMountains.clear();
    _mapRugged.clear();
    _planet.clear();
}

std::vector<float> Planet::regenerate()
{
    genPlanetLandmasses(0);
    genPlanetLFSlope(1);
    genPlanetMountains(2);
    genPlanetRugged(3);

    return getTerrain();
}

const std::vector<float>& Planet::bake()
{
    regenerate();

    std::vector<float> _landWaterWeighted = _mapLandWater;
    std::transform(_landWaterWeighted.begin(), _landWaterWeighted.end(), _landWaterWeighted.begin(), 
        std::bind(std::multiplies<float>(), std::placeholders::_1, _LandWater_Weight));

    std::vector<float> _slopeWeighted = _mapSlope;
    std::transform(_slopeWeighted.begin(), _slopeWeighted.end(), _slopeWeighted.begin(), 
        std::bind(std::multiplies<float>(), std::placeholders::_1, _SlopeBase_Weight));

    std::vector<float> _mountainsWeighted = _mapMountains;
    std::transform(_mountainsWeighted.begin(), _mountainsWeighted.end(), _mountainsWeighted.begin(), 
        std::bind(std::multiplies<float>(), std::placeholders::_1, _Mountains_Weight));

    std::vector<float> _ruggedWeighted = _mapRugged;
    std::transform(_ruggedWeighted.begin(), _ruggedWeighted.end(), _ruggedWeighted.begin(), 
        std::bind(std::multiplies<float>(), std::placeholders::_1, _Rugged_Weight));

    _planet.resize(_mapSideSize*_mapSideSize);
    for(size_t i = 0; i < _mapSideSize*_mapSideSize; ++i) {
        _planet[i] = _landWaterWeighted[i] + _slopeWeighted[i] + _mountainsWeighted[i] + _ruggedWeighted[i];
    }

    return _planet;
}


const std::vector<float>& Planet::getTerrain() const
{
    return _planet;
}

void Planet::bindCallbacks(FluxLumina& engine)
{
    engine.bindUserInput(GLFW_KEY_I, std::bind(&Planet::move, this, 0.0f, 0.1f));
    engine.bindUserInput(GLFW_KEY_K, std::bind(&Planet::move, this, 0.0f, -0.1f));
    engine.bindUserInput(GLFW_KEY_J, std::bind(&Planet::move, this, -0.1f, 0.0f));
    engine.bindUserInput(GLFW_KEY_L, std::bind(&Planet::move, this, 0.1f, 0.0f));

    engine.bindUserInput(GLFW_KEY_U, std::bind(&Planet::zoom, this, 1.01f));
    engine.bindUserInput(GLFW_KEY_O, std::bind(&Planet::zoom, this, 0.99f));
}

void Planet::zoom(float factor)
{
    _globalscale *= factor;

    if(_globalscale < 0.1f)
    {
        _globalscale = 0.1f;
    }
    else if(_globalscale > 10.0f)
    {
        _globalscale = 10.0f;
    }
}

void Planet::move(float x, float y)
{
    // _hOffset += x;
    // _vOffset += y;
}

const std::array<int, 2>& Planet::getChunkPosition() const
{
    return _worldGridPosition;
}

std::array<unsigned int, 2> Planet::getDimensions() const
{
    std::array<unsigned int, 2> result({_mapSideSize, _mapSideSize});
    return result;
}

/////// TERRAIN GENERATION ///////

// Mountains
void Planet::genPlanetLandmasses(int seedOffset)
{
    int size = static_cast<int>(_mapSideSize);
    std::array<int, 2> squareCenter = {_worldGridPosition[0] * size, _worldGridPosition[1] * size};

    _mapLandWater = genLandmasses(_seed+seedOffset, _mapSideSize, _worldGridPosition, _globalscale, _waterPercentage);

    _mapLandWaterFilter = _mapLandWater;

    std::transform(_mapLandWater.begin(), _mapLandWater.end(), _mapLandWaterFilter.begin(),
        [] (float val) {return val != 0.0f ? 1.0f : 0.0f;}
    );
}

void Planet::setLandWaterWeight(float weight)
{
    _LandWater_Weight = weight;
}

// LF Slope
void Planet::genPlanetLFSlope(int seedOffset)
{
    int size = static_cast<int>(_mapSideSize);
    std::array<int, 2> squareCenter = {_worldGridPosition[0] * size, _worldGridPosition[1] * size};
    _mapSlopeBase = genLandLFDetail(_seed+seedOffset, _mapSideSize, squareCenter, _globalscale);

    _mapSlope = _mapSlopeBase;

    std::transform(_mapSlopeBase.begin(), _mapSlopeBase.end(), _mapLandWater.begin(), _mapSlope.begin(), std::multiplies<float>());
}

void Planet::setSlopeWeight(float weight)
{
    _SlopeBase_Weight = weight;
}

// Mountains
void Planet::genPlanetMountains(int seedOffset)
{
    int size = static_cast<int>(_mapSideSize);
    std::array<int, 2> squareCenter = {_worldGridPosition[0] * size, _worldGridPosition[1] * size};
    _mapMountainsBase = genMountains(_seed+seedOffset, _mapSideSize, squareCenter, _globalscale);

    _mapMountains = _mapMountainsBase;

    for(int i(0); i < 7; ++i)
    {
    std::transform(_mapSlopeBase.begin(), _mapSlopeBase.end(), _mapMountains.begin(), _mapMountains.begin(), std::multiplies<float>());
    }

    std::transform(_mapLandWaterFilter.begin(), _mapLandWaterFilter.end(), _mapMountains.begin(), _mapMountains.begin(), std::multiplies<float>());

    // re-normalize 
    float max = *std::max_element(_mapMountains.begin(), _mapMountains.end());

    std::transform(_mapMountains.begin(), _mapMountains.end(), _mapMountains.begin(),
        std::bind(std::divides<float>(), std::placeholders::_1, max));

}

void Planet::setMountainsWeight(float weight)
{
    _Mountains_Weight = weight;
}

// Roughness
void Planet::genPlanetRugged(int seedOffset)
{
    int size = static_cast<int>(_mapSideSize);
    std::array<int, 2> squareCenter = {_worldGridPosition[0] * size, _worldGridPosition[1] * size};
    _mapRugged = genRoughness(_seed+seedOffset, _mapSideSize, squareCenter, _globalscale);

    float max = *std::max_element(_mapMountainsBase.begin(), _mapMountainsBase.end());

    for(int i(0); i < 3; ++i)
    {
    std::transform(_mapMountainsBase.begin(), _mapMountainsBase.end(), _mapRugged.begin(), _mapRugged.begin(), std::multiplies<float>());
    }

    std::transform(_mapLandWaterFilter.begin(), _mapLandWaterFilter.end(), _mapRugged.begin(), _mapRugged.begin(), std::multiplies<float>());

}

void Planet::setRuggedWeight(float weight)
{
    _Rugged_Weight = weight;
}
