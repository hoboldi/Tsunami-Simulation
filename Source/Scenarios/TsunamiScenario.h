#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <netcdf.h>
#include <netcdf>
#include <netcdfcpp.h>
#include <string>
#include <utility>
#include <vector>

#include "Scenario.hpp"

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
    void   setEndSimulationTime(double time);


    RealType     getBoundaryPos(BoundaryEdge edge) const override;
    void         readScenario(std::string bathymetry, std::string displacement) const;

  private:
    double endSimulationTime;
  };

} // namespace Scenarios