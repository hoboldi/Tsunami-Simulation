/**
 * @file
 * This file is part of SWE.
 *
 * @author Michael Bader, Kaveh Rahnema, Tobias Schnabel
 * @author Sebastian Rettenberger (rettenbs AT in.tum.de,
 * http://www5.in.tum.de/wiki/index.php/Sebastian_Rettenberger,_M.Sc.)
 *
 * @section LICENSE
 *
 * SWE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SWE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SWE.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * @section DESCRIPTION
 *
 * TODO
 */

#include "RadialDamBreakScenario.hpp"

#include <cmath>

RealType Scenarios::RadialDamBreakScenario::getWaterHeight(RealType x, RealType y) const {
  if(x >= 400 && x <= 600 && y >= 200 && y <= 300) {
    return 0;
  }
  return RealType((sqrt((x - 500.0) * (x - 500.0) + (y - 500.0) * (y - 500.0)) < 100.0) ? 15.0 : 10.0);
}

RealType Scenarios::RadialDamBreakScenario::getBathymetry([[maybe_unused]] RealType x, [[maybe_unused]] RealType y)
  const {
  if(x >= 400 && x <= 600 && y >= 200 && y <= 300) {
    return 10;
  }
  else{
    return -10;
  }
}

double Scenarios::RadialDamBreakScenario::getEndSimulationTime() const { return double(30); }

BoundaryType Scenarios::RadialDamBreakScenario::getBoundaryType([[maybe_unused]] BoundaryEdge edge) const {
  return BoundaryType::Outflow;
}

RealType Scenarios::RadialDamBreakScenario::getBoundaryPos(BoundaryEdge edge) const {
  if (edge == BoundaryEdge::Left) {
    return RealType(0.0);
  } else if (edge == BoundaryEdge::Right) {
    return RealType(1000.0);
  } else if (edge == BoundaryEdge::Bottom) {
    return RealType(0.0);
  } else {
    return RealType(1000.0);
  }
}
