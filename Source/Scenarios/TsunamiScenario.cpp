#include "TsunamiScenario.h"


struct interval {
  RealType xleft;
  RealType xright;
  RealType yleft;
  RealType yright;
  RealType h;
  RealType b;
};



std::vector<std::vector<interval>> intervalsOriginal;
std::vector<std::vector<interval>> intervalsDisplaced;
size_t indexi;
size_t indexj;

double calculateAvgOf(const double left, const double right) {
  return (left + right) * 0.5;
}


//TODO CHECKS
void Scenarios::TsunamiScenario::readScenario(std::string bathymetry, std::string displacement) const {
  //Read Bathymetry
  netCDF::NcFile bFile(bathymetry, netCDF::NcFile::read);

  //Get the variables
  netCDF::NcVar bxVar = bFile.getVar("x");
  netCDF::NcVar byVar = bFile.getVar("y");
  netCDF::NcVar bzVar = bFile.getVar("z");

  //Get Dimensions
  netCDF::NcDim bdimx = bxVar.getDim(0);
  netCDF::NcDim bdimy = byVar.getDim(0);

  std::vector<double> bxData(bdimx.getSize());
  std::vector<double> byData(bdimy.getSize());
  std::vector<double> bzData(bdimx.getSize() * bdimy.getSize());

  bxVar.getVar(&bxData[0]);
  byVar.getVar(&byData[0]);
  bzVar.getVar(&bzData[0]);

  ///////////////////////
  //Read Displacement
  netCDF::NcFile dFile(displacement, netCDF::NcFile::read);

  //Get the variables
  netCDF::NcVar dxVar = dFile.getVar("x");
  netCDF::NcVar dyVar = dFile.getVar("y");
  netCDF::NcVar dzVar = dFile.getVar("z");

  //Get Dimensions
  netCDF::NcDim ddimx = dxVar.getDim(0);
  netCDF::NcDim ddimy = dyVar.getDim(0);

  std::vector<double> dxData(ddimx.getSize());
  std::vector<double> dyData(ddimy.getSize());
  std::vector<double> dzData(ddimx.getSize() * ddimy.getSize());

  dxVar.getVar(&dxData[0]);
  dyVar.getVar(&dyData[0]);
  dzVar.getVar(&dzData[0]);


  RealType lastX = -std::numeric_limits<double>::infinity();
  RealType lastY = -std::numeric_limits<double>::infinity();


  //Calculate Original Intervals
  for(size_t i = 0; i < bxData.size() - 1; ++i) {
    std::vector<interval> oneGrid;
    for(size_t j = 0; j < byData.size() - 1; ++j) {
      interval oneInterval;
      oneInterval.xleft = lastX;
      oneInterval.xright = (bxData[i] + bxData[i]) * 0.5;
      oneInterval.yleft = lastY;
      oneInterval.yright = (byData[j] + byData[j]) * 0.5;
      oneInterval.b = bzData[i * byData.size() + j];
      oneInterval.h = -fmin(oneInterval.b,0);
    }
  }




}

RealType Scenarios::TsunamiScenario::getWaterHeight(RealType x, RealType y) const {
  return 0;
}

RealType Scenarios::TsunamiScenario::getBathymetry([[maybe_unused]] RealType x, [[maybe_unused]] RealType y)
 const {
 return 0;
}

double Scenarios::TsunamiScenario::getEndSimulationTime() const {
 return double(15);
}

BoundaryType Scenarios::TsunamiScenario::getBoundaryType([[maybe_unused]] BoundaryEdge edge) const {
 return BoundaryType::Outflow;
}

RealType Scenarios::TsunamiScenario::getBoundaryPos(BoundaryEdge edge) const {
 if (edge == BoundaryEdge::Left) {
   return RealType(0.0);
 } else if (edge == BoundaryEdge::Right) {
   return 0;
 } else if (edge == BoundaryEdge::Bottom) {
   return RealType(0.0);
 } else {
   return 0;
 }
}
