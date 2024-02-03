#pragma once

#include <array>
#include <vector>
#include <random>

class WaterDrop;

class HydroEroder
{
public:
    HydroEroder(std::vector<float>& map, unsigned int sidesize);

    std::vector<float> getResult();
    std::vector<float> getDelta();

    void drop(unsigned int numberOfDrops);

private:

    void updateDrop(WaterDrop& drop);

    std::array<float, 2> calculatePositionGradient(std::array<float, 2> position);
    float calculatePositionHeight(std::array<float, 2> position);

    void depositSediment(std::array<float, 2>, WaterDrop& drop, float maxSediment);
    void collectSediment(std::array<float, 2>, WaterDrop& drop, float maxSediment);

    unsigned int _sideSize;
    std::vector<float>& _map;
    std::vector<float> _erosionDelta;

    // Erosion parameters
    float _inertia, _minimalSlope, _gravity;
    const float _erosionRadius, _erosionFactor, _depositionFactor;

    // Random generator
    std::random_device rd;
    std::mt19937 rng; // Mersenne Twister random number generator
    std::uniform_real_distribution<float> dist;
};


struct WaterDrop
{
public:
    WaterDrop(float x, float y);
    void setDirection(float x, float y);

    std::array<float, 2> _position;
    std::array<float, 2> _direction;
    float _speed, _waterMass, _sedimentMass, _maxCapacity;
    const float _capacityFactor, _evaporationFactor;
    unsigned int _ttl;

};