#include "erosion/hydro.hpp"

#include <algorithm>
#include <numeric>
#include <cmath>

namespace
{
    inline unsigned int getArrayIndex(unsigned int x, unsigned int y, unsigned int sideSize)
    {
        return y * sideSize + x;
    }

    inline std::array<unsigned int, 2> getArrayXY(unsigned int index, unsigned int sideSize)
    {
        return {index % sideSize, index / sideSize};
    }

    // Generate a random float between 0 and 1
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<float> dist01(0, 1);
    
}


HydroEroder::HydroEroder(std::vector<float>& map, unsigned int sidesize)
    : _map(map), 
    _sideSize(sidesize),
    _erosionRadius(5.0f),
    _depositionFactor(0.1f),
    _erosionFactor(0.5f),
    _minimalSlope(0.01f),
    _inertia(0.025f),
    _gravity(4.0f)
{

    rng = std::mt19937(rd());
    dist = std::uniform_real_distribution<float>(0, _sideSize - 1);
}

std::vector<float> HydroEroder::getResult()
{
    return _map;
}

std::vector<float> HydroEroder::getDelta()
{
    return _erosionDelta;
}

void HydroEroder::drop(unsigned int numberOfDrops)
{
    for(unsigned int i = 0; i < numberOfDrops; ++i)
    {
        WaterDrop drop = WaterDrop({dist(rng), dist(rng)});
        while(drop._ttl > 0)
        {
            updateDrop(drop);
        }
    }
}

void HydroEroder::updateDrop(WaterDrop& drop)
{

    std::array<float, 2> gradient = calculatePositionGradient(drop._position);

    drop.setDirection(  drop._direction[0] * _inertia - gradient[0] * (1.0f - _inertia),
                        drop._direction[1] * _inertia - gradient[1] * (1.0f - _inertia));

    std::array<float, 2> newPosition = {drop._position[0] + drop._direction[0], drop._position[1] + drop._direction[1]};

    // If the drop is out of the map, remove it
    if(newPosition[0] < 0 || newPosition[0] >= _sideSize - 1 || newPosition[1] < 0 || newPosition[1] >= _sideSize - 1)
    {
        drop._ttl = 0;
        return;
    }

    float hDelta = calculatePositionHeight(newPosition) - calculatePositionHeight(drop._position);

    if (hDelta > 0)
    {
        depositSediment(drop._position, drop, std::min(hDelta, drop._sedimentMass));
    }
    else
    {
        drop._maxCapacity = std::max(-hDelta, _minimalSlope) * drop._speed * drop._waterMass * drop._capacityFactor;

        if(drop._sedimentMass < drop._maxCapacity)
        {
            collectSediment(drop._position, drop, std::min(drop._maxCapacity - drop._sedimentMass, -hDelta));
        }
        else
        {
            depositSediment(drop._position, drop, (drop._sedimentMass - drop._maxCapacity) * _depositionFactor);
        }
    }



    drop._speed = std::sqrt(std::abs(drop._speed * drop._speed + hDelta * _gravity));
    drop._waterMass *= (1.0f - drop._evaporationFactor);
    drop._ttl--;
    drop._position = newPosition;
};

std::array<float, 2> HydroEroder::calculatePositionGradient(std::array<float, 2> position)
{
    unsigned int x0, x1, y0, y1;

    // The coodinates of the 4 points surrounding the position
    x0 = static_cast<unsigned int>(position[0]);
    x1 = std::min(x0 + 1, _sideSize - 1);
    y0 = static_cast<unsigned int>(position[1]);
    y1 = std::min(y0 + 1, _sideSize - 1);

    // The values of the 4 points surrounding the position
    float Vx0y0 = _map[getArrayIndex(x0, y0, _sideSize)];
    float Vx1y0 = _map[getArrayIndex(x1, y0, _sideSize)];
    float Vx0y1 = _map[getArrayIndex(x0, y1, _sideSize)];
    float Vx1y1 = _map[getArrayIndex(x1, y1, _sideSize)];

    // The decimal part of the position
    float xDec = position[0] - x0;
    float yDec = position[1] - y0;

    // The gradient of the 4 points surrounding the position
    std::array<float, 2> gX0Y0 = {  Vx1y0 - Vx0y0, 
                                    Vx0y1 - Vx0y0};

    std::array<float, 2> gX1Y0 = {  Vx1y0 - Vx0y0,
                                    Vx1y1 - Vx1y0};

    std::array<float, 2> gX0Y1 = {  Vx1y1- Vx0y1,
                                    Vx0y1 - Vx0y0};

    std::array<float, 2> gX1Y1 = {  Vx1y1 - Vx0y1,
                                    Vx1y1 - Vx1y0};

    // The gradient of the position via bilinear interpolation
    std::array<float, 2> gradient = {  
    (1.0f - xDec) * (1.0f - yDec) * gX0Y0[0] + xDec * (1.0f - yDec) * gX1Y0[0] + (1.0f - xDec) * yDec * gX0Y1[0] + xDec * yDec * gX1Y1[0],
    (1.0f - xDec) * (1.0f - yDec) * gX0Y0[1] + xDec * (1.0f - yDec) * gX1Y0[1] + (1.0f - xDec) * yDec * gX0Y1[1] + xDec * yDec * gX1Y1[1]};

    return gradient;
}

float HydroEroder::calculatePositionHeight(std::array<float, 2> position)
{
    unsigned int x0, x1, y0, y1;

    // The coodinates of the 4 points surrounding the position
    x0 = static_cast<unsigned int>(position[0]);
    x1 = std::min(x0 + 1, _sideSize - 1);
    y0 = static_cast<unsigned int>(position[1]);
    y1 = std::min(y0 + 1, _sideSize - 1);

    // The values of the 4 points surrounding the position
    float Vx0y0 = _map[getArrayIndex(x0, y0, _sideSize)];
    float Vx1y0 = _map[getArrayIndex(x1, y0, _sideSize)];
    float Vx0y1 = _map[getArrayIndex(x0, y1, _sideSize)];
    float Vx1y1 = _map[getArrayIndex(x1, y1, _sideSize)];

    // The decimal part of the position
    float xDec = position[0] - x0;
    float yDec = position[1] - y0;

    // The height of the position via bilinear interpolation
    return (1.0f - xDec) * (1.0f - yDec) * Vx0y0 + xDec * (1.0f - yDec) * Vx1y0 + (1.0f - xDec) * yDec * Vx0y1 + xDec * yDec * Vx1y1;
}

void HydroEroder::depositSediment(std::array<float, 2> position, WaterDrop& drop, float sedimentToDeposit)
{
    unsigned int x0, x1, y0, y1;

    // The coodinates of the 4 points surrounding the position
    x0 = static_cast<unsigned int>(position[0]);
    x1 = std::min(x0 + 1, _sideSize - 1);
    y0 = static_cast<unsigned int>(position[1]);
    y1 = std::min(y0 + 1, _sideSize - 1);

    // The weights of the 4 points surrounding the position
    float xDec = position[0] - x0;
    float yDec = position[1] - y0;

    float wX0Y0 = (1.0f - xDec) * (1.0f - yDec);
    float wX1Y0 = xDec * (1.0f - yDec);
    float wX0Y1 = (1.0f - xDec) * yDec;
    float wX1Y1 = xDec * yDec;

    // The sediment to deposit on the 4 points surrounding the position
    float sX0Y0 = wX0Y0 * sedimentToDeposit;
    float sX1Y0 = wX1Y0 * sedimentToDeposit;
    float sX0Y1 = wX0Y1 * sedimentToDeposit;
    float sX1Y1 = wX1Y1 * sedimentToDeposit;

    // Deposit the sediment
    _map[getArrayIndex(x0, y0, _sideSize)] += sX0Y0;
    _map[getArrayIndex(x1, y0, _sideSize)] += sX1Y0;
    _map[getArrayIndex(x0, y1, _sideSize)] += sX0Y1;
    _map[getArrayIndex(x1, y1, _sideSize)] += sX1Y1;

    // Update the drop's sediment mass
    drop._sedimentMass -= sedimentToDeposit;
}

void HydroEroder::collectSediment(std::array<float, 2> position, WaterDrop& drop, float amount)
{
    std::vector<float> collectionWeights;

    unsigned int Xstart = std::max(0.0f, std::ceil(position[0] - _erosionRadius));
    unsigned int Xend = std::min(std::floor(position[0] + _erosionRadius), static_cast<float>(_sideSize - 1));
    unsigned int Ystart = std::max(0.0f, std::ceil(position[1] - _erosionRadius));
    unsigned int Yend = std::min(std::floor(position[1] + _erosionRadius), static_cast<float>(_sideSize - 1));

    for (unsigned int x = Xstart; x <= Xend; ++x)
    {
        for (unsigned int y = Ystart; y <= Yend; ++y)
        {
            float distance = sqrt((position[0] - x) * (position[0] - x) + (position[1] - y) * (position[1] - y));
            float weight = std::max(0.0f, _erosionRadius - distance);
            collectionWeights.push_back(weight);
        }
    }

    float totalWeight = std::accumulate(collectionWeights.begin(), collectionWeights.end(), 0.0f);

    auto collection_weights_it = collectionWeights.begin();

    for (unsigned int x = Xstart; x <= Xend; ++x)
    {
        for (unsigned int y = Ystart; y <= Yend; ++y)
        {
            float transferAmount = amount * _erosionFactor * *collection_weights_it / totalWeight;
            _map[getArrayIndex(x, y, _sideSize)] -= transferAmount;
            drop._sedimentMass += transferAmount;
            ++collection_weights_it;
        }
    }

}

WaterDrop::WaterDrop(float x, float y)    :
    _position({x, y}),
    _direction({0.0f, 0.0f}),
    _speed(1.0f),
    _waterMass(1.0f),
    _sedimentMass(0.0f),
    _ttl(50),
    _maxCapacity(0.0f),
    _capacityFactor(8.0f),
    _evaporationFactor(0.05f)
{
    ;
}

void WaterDrop::setDirection(float x, float y)
{
    // normalize the direction
    float magnitude = sqrt(x * x + y * y);
    if (magnitude != 0) // Check to avoid division by zero
        _direction = {x / magnitude, y / magnitude};
    else    // If direction vector was going to be null, set it to a random direction
        setDirection(dist01(rng), dist01(rng));
}
