/**
 * @file
 * This file is part of SWE.
 *
 * @author Michael Bader, Kaveh Rahnema, Tobias Schnabel
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

#include "Scenario.hpp"

#include <iostream>


RealType Scenarios::Scenario::getWaterHeight([[maybe_unused]] RealType x, [[maybe_unused]] RealType y) const {
  return RealType(10.0);
}

RealType Scenarios::Scenario::getVelocityU([[maybe_unused]] RealType x, [[maybe_unused]] RealType y) const {
  return RealType(0.0);
}

RealType Scenarios::Scenario::getVelocityV([[maybe_unused]] RealType x, [[maybe_unused]] RealType y) const {
  return RealType(0.0);
}

RealType Scenarios::Scenario::getBathymetry([[maybe_unused]] RealType x, [[maybe_unused]] RealType y) const {
  return RealType(0.0);
}

RealType Scenarios::Scenario::getWaterHeightAtRest() const { return RealType(10.0); }

double Scenarios::Scenario::getEndSimulationTime() const { return 0.1; }

BoundaryType Scenarios::Scenario::getBoundaryType([[maybe_unused]] BoundaryEdge edge) const {
  return BoundaryType::Wall;
}

void Scenarios::Scenario::setEndSimulationTime(double time) {
  assert(time >= 0);
  endSimulationTime = time;
}

void Scenarios::Scenario::setBoundaryType(int type) {

  int left = type / 1000;
  int right = (type % 1000) / 100;
  int bottom = (type % 100) / 10;
  int top = type % 10;
  if(left == 1) {
    boundaryTypeLeft = BoundaryType::Outflow;
  } else if(left == 2) {
    boundaryTypeLeft = BoundaryType::Wall;
  }if(right == 1){
    boundaryTypeRight = BoundaryType::Outflow;
  } else if(right == 2) {
    boundaryTypeRight = BoundaryType::Wall;
  }if(bottom == 1) {
    boundaryTypeBottom = BoundaryType::Outflow;
  } else if(bottom == 2) {
    boundaryTypeBottom = BoundaryType::Wall;
  }if(top == 1) {
    boundaryTypeTop = BoundaryType::Outflow;
  } else if(top == 2) {
    boundaryTypeTop = BoundaryType::Wall;
  }
}

RealType Scenarios::Scenario::getBoundaryPos(BoundaryEdge edge) const {
  if (edge == BoundaryEdge::Left || edge == BoundaryEdge::Bottom) {
    return RealType(0.0);
  } else {
    return RealType(1.0);
  }
}
RealType Scenarios::Scenario::getStartTime() const { return 0; }
