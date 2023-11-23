#pragma once

#include "Scenario.hpp"
#include <cmath>
#include <vector>
#include <string>
#include <netcdf.h>
#include <netcdf>
#include <netcdfcpp.h>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <utility>
#include <array>
#include <cfloat>

#include "Readers/NetCDFReader.h"
#include "Tools/Float2D.hpp"

namespace Scenarios {

  /**
   * Scenario "Tsunami Scenario":
   * TODO
  */
  class TsunamiScenario: public Scenario {
  public:
    TsunamiScenario(const std::string& bathymetry, const std::string& displacement);
    ~TsunamiScenario() override = default;

    RealType getWaterHeight(RealType x, RealType y) const override;
    RealType getBathymetry(RealType x, RealType y) const override;

    double getEndSimulationTime() const override;
    void setEndSimulationTime(double time);

    BoundaryType getBoundaryType(BoundaryEdge edge) const override;
    void setBoundaryType(int type);
    RealType     getBoundaryPos(BoundaryEdge edge) const override;
  private:
    Tools::Float2D<RealType> bathymetry_;
    Tools::Float2D<RealType> water_height_;
    mutable int nx_;
    mutable int ny_;
    mutable RealType dx_;
    mutable RealType dy_;
    BoundaryType boundaryType;
    double endSimulationTime;

  };

} // namespace Scenarios