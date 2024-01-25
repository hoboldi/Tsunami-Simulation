#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <netcdf.h>
#include <netcdf>
#include <string>
#include <utility>
#include <vector>
#include "Tools/Interval.h"

#include "Scenario.hpp"

typedef Tools::interval interval;

namespace Scenarios {

  std::vector<std::vector<interval>> getInterval();

  /**
   * Scenario "Tsunami Scenario":
   * NetCDF Read Scenario
   */
  class TsunamiScenario: public Scenario {
  private:
    
  public:
    ~TsunamiScenario() override = default;

    RealType getWaterHeight(RealType x, RealType y) const override;
    RealType getBathymetry(RealType x, RealType y) const override;

    double getEndSimulationTime() const override;
    void   setEndSimulationTime(double time);


    RealType     getBoundaryPos(BoundaryEdge edge) const override;

    /**
       * @brief A method to read from two netcdf files oen for bathymetry and for displacement.
       *
       * @param [in] bathymetry: The name of the file which will be read -> The bathymetry file
       * @param [in] displacement: The name of the variable which will be read -> The displacement file
     */
    void         readScenario(std::string bathymetry, std::string displacement) const;

  private:
    double endSimulationTime;
  };

} // namespace Scenarios