#include <string>

#include "Readers/NetCDFUnbufferedReader.h"
#include "Scenario.hpp"
namespace Scenarios {
  class FileScenario : public Scenario {
    public:
      explicit FileScenario(const std::string& bathymetry, int numCellsX, int numCellsY, int offsetX, RealType epicenterX, RealType epicenterY, RealType magnitude);

      RealType getBathymetry(RealType x, RealType y) const override;
      RealType getWaterHeight(RealType x, RealType y) const override;

      RealType getBoundaryPos(BoundaryEdge edge) const override;

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

      void setEpicenter(RealType x, RealType y) override{
        epicenterX = x;
        epicenterY = y;
      }

      void setMagnitude(RealType magnitude) {
        this->magnitude = magnitude;
      }

    private:
      RealType epicenterX;
      RealType epicenterY;
      RealType magnitude;
      RealType startingWaveHeight;

      Readers::NetCDFUnbufferedReader reader_;
      double xDim, yDim;
      int dx_, dy_;
      int offsetX_;
      int numCellsX, numCellsY;
  };
} // namespace Scenarios
