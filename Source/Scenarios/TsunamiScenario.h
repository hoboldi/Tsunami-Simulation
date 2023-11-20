#pragma once

#include "Scenario.hpp"
#include <cmath>
#include <vector>
#include <string>
#include <netcdf>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <utility>
#include <array>

namespace Scenarios {

  /**
   * Scenario "Tsunami Scenario":
   * TODO
  */
  class TsunamiScenario: public Scenario {
  public:
    ~TsunamiScenario() override = default;

    RealType getWaterHeight(RealType x, RealType y) const override;
    RealType getBathymetry(RealType x, RealType y) const override;

    double getEndSimulationTime() const override;

    BoundaryType getBoundaryType(BoundaryEdge edge) const override;
    RealType     getBoundaryPos(BoundaryEdge edge) const override;
    void         readScenario(std::string bathymetry, std::string displacement) const;
  };

} // namespace Scenarios