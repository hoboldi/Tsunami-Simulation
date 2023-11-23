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
  class CheckpointScenario: public Scenario {
  public:
    CheckpointScenario(const std::string& filename);
    ~CheckpointScenario() override = default;

    RealType getWaterHeight(RealType x, RealType y) const override;
    RealType getBathymetry(RealType x, RealType y) const override;
    RealType getDischargeHu(RealType x, RealType y) const override;
    RealType getDischargeHv(RealType x, RealType y) const override;

    BoundaryType getBoundaryType(BoundaryEdge edge) const override;
    int getBoundaryTypeForInteger(int value) const override;
    RealType getBoundaryPos(BoundaryEdge edge) const override;
  private:
    Tools::Float2D<RealType> bathymetry_;
    Tools::Float2D<RealType> height_;
    Tools::Float2D<RealType> momentaX_;
    Tools::Float2D<RealType> momentaY_;
    mutable int nx_;
    mutable int ny_;
    mutable RealType dx_;
    mutable RealType dy_;
    int timestepStart;
    mutable RealType timePassedAtStart;
    BoundaryType boundaryType;
    double endSimulationTime;
    int boundaries[];

  };

} // namespace Scenarios