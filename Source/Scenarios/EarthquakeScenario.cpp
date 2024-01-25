

#include "EarthquakeScenario.h"

#include <cmath>

Scenarios::EarthquakeScenario::EarthquakeScenario(int epicenterX, int epicenterY, RealType magnitude, RealType bathymetryInEpicenter)
{
    this->epicenterX = epicenterX;
    this-> epicenterY = epicenterY;
    this->magnitude = magnitude;
    this->heightInEpicenter = bathymetryInEpicenter;
}

RealType Scenarios::EarthquakeScenario::getStartingWaveHeight() const
{
    
}

RealType Scenarios::EarthquakeScenario::getMaxWaveHeight() const
{
    if (magnitude < 6.51)
    {
        return 0;
    }
    else
    {
        return (7,6875 * magnitude) - 50,0417;
    }
}

RealType Scenarios::EarthquakeScenario::getWaterHeight(RealType x, RealType y) const {
  // Bathymetry before earthquake = -100m -> Waterheight = 100m
  return 100;
}

RealType Scenarios::EarthquakeScenario::getBathymetry([[maybe_unused]] RealType x, [[maybe_unused]] RealType y) const {
  // constant bathymetry of -100m before earthquake

  // calculate displacement in 1km square around epicenter
  if (x >= 4500 && x <= 5500 && y >= 4500 && y <= 5500) {
    RealType x_Displacement = x - 5000;
    RealType y_Displacement = y - 5000;
    RealType BathymetryAfterEarthquake = -100 + calculateDisplacement(x_Displacement, y_Displacement);

    // No Bathymetry values between -20 and 20
    if (BathymetryAfterEarthquake > -20 && BathymetryAfterEarthquake <= 0) {
      return -20;
    } else if (BathymetryAfterEarthquake > 0 && BathymetryAfterEarthquake < 20) {
      return 20;
    } else {
      return BathymetryAfterEarthquake;
    }
  } else {
    return -100;
  }
}

RealType Scenarios::EarthquakeScenario::calculateDisplacement(RealType x, RealType y) const {
  RealType d_x = sin(((x / 500) + 1) * PI);
  RealType d_y = -pow(y / 500, 2) + 1;
  return 5 * d_x * d_y;
}

double Scenarios::EarthquakeScenario::getEndSimulationTime() const { return double(100); }

BoundaryType Scenarios::EarthquakeScenario::getBoundaryType([[maybe_unused]] BoundaryEdge edge) const { return BoundaryType::Wall; }

RealType Scenarios::EarthquakeScenario::getBoundaryPos(BoundaryEdge edge) const {
  if (edge == BoundaryEdge::Left) {
    return RealType(0.0);
  } else if (edge == BoundaryEdge::Right) {
    return RealType(10000.0);
  } else if (edge == BoundaryEdge::Bottom) {
    return RealType(0.0);
  } else {
    return RealType(10000.0);
  }
}
