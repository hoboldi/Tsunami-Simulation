
#pragma once

#include "Scenario.hpp"

#define PI 3.14159265

namespace Scenarios {

 class EarthquakeScenario: public Scenario {
 private:
    int epicenterX;
    int epicenterY;
    RealType magnitude;
    RealType startingWaveHeight;
    RealType heightInEpicenter;
 public:
 //Important: We calculate using the moment magnitude Mw, not the magnitude of the richter scale, since it's more accurate and prominent in scientific circles
    EarthquakeScenario(int epicenterX, int epicenterY, RealType Magnitude, RealType bathymetryInEpicenter);
   ~EarthquakeScenario() override = default;

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

   /**
    * @brief This method takes the maxWaveHeight and uses Green's Law to estimate the starting wave height we need to start the process at the epicenter of the earthquake
    * We use a method that approximates the max wave height to be reached at a water height of 50m offshore, therefore the wave being close to land alredy.
    * This is beacause Green's Law becomes less precise the closer it get's to the shore.
    * On top of that, we would definetly rather give the warning a little too early, than too late.
    * 
    * @return RealType 
    */
   RealType getStartingWaveHeight() const;

   /**
    * @brief A Method used to calculate the estimated max wave height based on data found in https://www.scirp.org/journal/paperinformation?paperid=76149#:~:text=The%20tsunami%20heights%20documented%20correspond,to%20high%2Dmagnitude%20seismic%20events.
    * Note that it only works for magnitudes >= 6,51, for anything lower than that the formula auto-returns a wave height of 0. This is also usually the point where tsunamis become unlikely, altough not impossible
    * After extrapolating the data points, we get the formula: height = 7,6875 * magnitude – 50,0417
    * 
    * @return height: The max wave height calculated 
    */
   RealType getMaxWaveHeight() const;
 };

} // namespace Scenarios
