#include <string>

#include "Readers/NetCDFUnbufferedReader.h"
#include "Scenario.hpp"
namespace Scenarios {
  class FileScenario : public Scenario {
    public:
      explicit FileScenario(const std::string& bathymetry, int numCellsX, int numCellsY, int offsetX);

      RealType getBathymetry(RealType x, RealType y) const override;
      RealType getWaterHeight(RealType x, RealType y) const override;

      RealType getBoundaryPos(BoundaryEdge edge) const override;

    private:
      Readers::NetCDFUnbufferedReader reader_;
      double xDim, yDim;
      int dx_, dy_;
      int offsetX_;
  };
} // namespace Scenarios
