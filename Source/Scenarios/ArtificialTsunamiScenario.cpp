

#include "ArtificialTsunamiScenario.h"

#include <cmath>

RealType Scenarios::ArtificialTsunamiScenario::getWaterHeight(RealType x, RealType y) const {
  // Bathymetry before earthquake = -100m -> Waterheight = 100m
  return 100;
}

RealType Scenarios::ArtificialTsunamiScenario::getBathymetry([[maybe_unused]] RealType x, [[maybe_unused]] RealType y) const {
  // constant bathymetry of -100m before earthquake

  // calculate displacement in 1km square around epicenter
  if (x >= 4500 && x <= 5500 && y >= 4500 && y <= 5500) {
    RealType BathymetryAfterEarthquake = -100 + calculateDisplacement(x, y);

    // No Bathymetry values between -20 and 20
    if (BathymetryAfterEarthquake > -20 && BathymetryAfterEarthquake <= 0) {
      return -20;
    } else if (BathymetryAfterEarthquake > 0 && BathymetryAfterEarthquake < 20) {
      return 20;
    }
  } else {
    return RealType(-100);
  }
}

RealType Scenarios::ArtificialTsunamiScenario::calculateDisplacement(RealType x, RealType y) const {
  RealType d_x = sin(((x / 500) + 1) * PI);
  RealType d_y = -pow(y / 500, 2) + 1;
  return 5 * d_x * d_y;
}

double Scenarios::ArtificialTsunamiScenario::getEndSimulationTime() const { return double(15); }

BoundaryType Scenarios::ArtificialTsunamiScenario::getBoundaryType([[maybe_unused]] BoundaryEdge edge) const { return BoundaryType::Outflow; }

RealType Scenarios::ArtificialTsunamiScenario::getBoundaryPos(BoundaryEdge edge) const {
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
