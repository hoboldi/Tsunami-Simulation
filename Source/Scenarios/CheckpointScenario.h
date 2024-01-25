#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cfloat>
#include <cmath>
#include <iostream>
#include <netcdf.h>
#include <netcdf>
#include <string>
#include <utility>
#include <vector>

#include "Readers/NetCDFReader.h"
#include "Scenario.hpp"
#include "Tools/Float2D.hpp"

namespace Scenarios {

  /**
   * Scenario "Tsunami Scenario":
   * TODO
   */
  class CheckpointScenario: public Scenario {
  public:
    explicit CheckpointScenario(const std::string& filename);
    ~CheckpointScenario() override = default;

    RealType getWaterHeight(RealType x, RealType y) const override;
    RealType getBathymetry(RealType x, RealType y) const override;
    RealType getVelocityU(RealType x, RealType y) const override;
    RealType getVelocityV(RealType x, RealType y) const override;

    static BoundaryType getBoundaryTypeForInteger(int value) ;
    RealType     getBoundaryPos(BoundaryEdge edge) const override;
    RealType     getStartTime() const override;

  private:
    RealType                 timestepStart;
    Tools::Float2D<RealType> bathymetry_;
    Tools::Float2D<RealType> height_;
    Tools::Float2D<RealType> momentaX_;
    Tools::Float2D<RealType> momentaY_;

    double*          bathymetries;
    double*          heights;
    double*          momentaX;
    double*          momentaY;
    mutable int      nx_;
    mutable int      ny_;
    mutable RealType dx_;
    mutable RealType dy_;

    mutable RealType timePassedAtStart;
    BoundaryType     boundaryType;
    double           endSimulationTime;
    int              boundaries;
  };

} // namespace Scenarios