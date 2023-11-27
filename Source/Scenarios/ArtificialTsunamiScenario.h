
#pragma once

#include "Scenario.hpp"

#define PI 3.14159265

namespace Scenarios {

 class ArtificialTsunamiScenario: public Scenario {
 public:
   ~ArtificialTsunamiScenario() override = default;

   RealType getWaterHeight(RealType x, RealType y) const override;
   RealType getBathymetry(RealType x, RealType y) const override;

   double getEndSimulationTime() const override;

   BoundaryType getBoundaryType(BoundaryEdge edge) const override;
   RealType     getBoundaryPos(BoundaryEdge edge) const override;

   /**
    * @brief Calculates the displacement of the bathymetry after the earthquake
    * @param x X-Coordinate
    * @param y Y-Coordinate
    * @return Effect of the displacement on the bathymetry
    */
   RealType calculateDisplacement([[maybe_unused]] RealType x, [[maybe_unused]] RealType y) const;
 };

} // namespace Scenarios
