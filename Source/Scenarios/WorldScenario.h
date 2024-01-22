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


  struct interval {
    RealType xleft;
    RealType xright;
    RealType yleft;
    RealType yright;
    RealType h;
    RealType b;
  };

  std::vector<std::vector<interval>> getInterval();


  namespace WorldScenario {
    class PreEarthquake: public Scenario {
    public:
      ~PreEarthquake() override = default;

      /**
       * @brief A method to read from two netcdf files oen for bathymetry and for displacement.
       *
       * @param [in] bathymetry: The name of the file which will be read -> The bathymetry file
       */
      void   readWorld(std::string bathymetry) const;

      RealType getWaterHeight(RealType x, RealType y) const override;
      RealType getBathymetry(RealType x, RealType y) const override;
    };

    class PostEarthquake: public Scenario {
    public:
      ~PostEarthquake() override = default;

      RealType getWaterHeight(RealType x, RealType y) const override;
      RealType getBathymetry(RealType x, RealType y) const override;

      double getEndSimulationTime() const override;
      void   setEndSimulationTime(double time);

      RealType getBoundaryPos(BoundaryEdge edge) const override;
    private:
      double endSimulationTime;
    };
  }



} // namespace Scenarios