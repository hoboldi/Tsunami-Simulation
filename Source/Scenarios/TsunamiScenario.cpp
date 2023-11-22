#include "TsunamiScenario.h"

#include <cfloat>


struct interval {
  RealType xleft;
  RealType xright;
  RealType yleft;
  RealType yright;
  RealType h;
  RealType b;
};



std::vector<std::vector<interval>> intervals;
unsigned long                      indexi = 0;
unsigned long                      indexj = 0;

double calculateAvgOf(const double left, const double right) {
  return (left + right) * 0.5;
}

void Scenarios::TsunamiScenario::readScenario(std::string bathymetry, std::string displacement) const {
  //Read Bathymetry
  int bncid, bvarid;
  int retval;

  retval = nc_open(bathymetry.c_str(),NC_NOWRITE,&bncid);
  assert(retval == NC_NOERR);

  //Get the variables
  int bx_dimid, by_dimid;
  int bx_varid, by_varid, bz_varid;
  size_t bxlen, bylen;

  //Get dimension ids
  retval = nc_inq_dimid(bncid, "x", &bx_dimid);
  assert(retval == NC_NOERR);
  retval = nc_inq_dimid(bncid, "y", &by_dimid);
  assert(retval == NC_NOERR);

  //Get variable ids
  retval = nc_inq_varid(bncid, "x", &bx_varid);
  assert(retval == NC_NOERR);
  retval = nc_inq_varid(bncid, "y", &by_varid);
  assert(retval == NC_NOERR);
  retval = nc_inq_varid(bncid, "z", &bz_varid);
  assert(retval == NC_NOERR);

  //Get dimension lengths
  retval = nc_inq_dimlen(bncid,bx_dimid, &bxlen);
  assert(retval == NC_NOERR);
  retval = nc_inq_dimlen(bncid,by_dimid, &bylen);
  assert(retval == NC_NOERR);

  float bx_data[bxlen];
  float by_data[bylen];
  float bz_data[bxlen * bylen];

  retval = nc_get_var_float(bncid,bx_varid,bx_data);
  assert(retval == NC_NOERR);
  retval = nc_get_var_float(bncid,by_varid,by_data);
  assert(retval == NC_NOERR);
  retval = nc_get_var_float(bncid,bz_varid,bz_data);
  assert(retval == NC_NOERR);

  std::vector<double> bxData(bxlen);
  std::vector<double> byData(bylen);
  std::vector<double> bzData(bxlen * bylen);


  for(size_t i = 0; i < bxlen; i++) {
    bxData[i] = bx_data[i];
  }
  for(size_t i = 0; i < bylen; i++) {
    byData[i] = by_data[i];
  }
  for(size_t i = 0; i < bxlen * bylen; i++) {
    bzData[i] = bz_data[i];
  }

  RealType lastX = -DBL_MAX;
  RealType lastY = -DBL_MAX;

  //Calculate Original Intervals TODO
  for(size_t i = 0; i < bxData.size(); ++i) {
    std::vector<interval> oneGrid;
    for(size_t j = 0; j < byData.size() - 1; ++j) {
      interval oneInterval;
      oneInterval.xleft = lastX;
      oneInterval.xright = (bxData[i] + bxData[i]) * 0.5;
      lastX = oneInterval.xright;
      oneInterval.yleft = lastY;
      oneInterval.yright = (byData[j] + byData[j]) * 0.5;
      lastY = oneInterval.yright;
      oneInterval.b = bzData[i * byData.size() + j];
      oneInterval.h = -fmin(oneInterval.b,0);
      oneGrid.push_back(oneInterval);
    }
    interval oneInterval;
    oneInterval.xleft = lastX;
    oneInterval.xright = DBL_MAX;
    oneInterval.yleft = lastY;
    oneInterval.yright = DBL_MAX;
    oneInterval.b = bzData[(i + 1) * byData.size() - 1];
    oneInterval.h = -fmin(oneInterval.b,0);
    oneGrid.push_back(oneInterval);
    intervals.push_back(oneGrid);
  }

  int dncid, dvarid;

  retval = nc_open(displacement.c_str(),NC_NOWRITE,&dncid);
  assert(retval == NC_NOERR);

  //Get the variables
  int dx_dimid, dy_dimid;
  int dx_varid, dy_varid, dz_varid;
  size_t dxlen, dylen;

  //Get dimension ids
  retval = nc_inq_dimid(dncid, "x", &dx_dimid);
  assert(retval == NC_NOERR);
  retval = nc_inq_dimid(dncid, "y", &dy_dimid);
  assert(retval == NC_NOERR);

  //Get variable ids
  retval = nc_inq_varid(dncid, "x", &dx_varid);
  assert(retval == NC_NOERR);
  retval = nc_inq_varid(dncid, "y", &dy_varid);
  assert(retval == NC_NOERR);
  retval = nc_inq_varid(dncid, "z", &dz_varid);
  assert(retval == NC_NOERR);

  //Get dimension lengths
  retval = nc_inq_dimlen(dncid,dx_dimid, &dxlen);
  assert(retval == NC_NOERR);
  retval = nc_inq_dimlen(dncid,dy_dimid, &dylen);
  assert(retval == NC_NOERR);

  float dx_data[dxlen];
  float dy_data[dylen];
  float dz_data[dxlen * dylen];

  retval = nc_get_var_float(dncid,dx_varid,dx_data);
  assert(retval == NC_NOERR);
  retval = nc_get_var_float(dncid,dy_varid,dy_data);
  assert(retval == NC_NOERR);
  retval = nc_get_var_float(dncid,dz_varid,dz_data);
  assert(retval == NC_NOERR);

  std::vector<double> dxData(dxlen);
  std::vector<double> dyData(dylen);
  std::vector<double> dzData(dxlen * dylen);


  for(size_t i = 0; i < dxlen; i++) {
    dxData[i] = dx_data[i];
  }
  for(size_t i = 0; i < dylen; i++) {
    dyData[i] = dy_data[i];
  }
  for(size_t i = 0; i < dxlen * dylen; i++) {
    dzData[i] = dz_data[i];
  }


  //Calculate Displaced Intervals
  for(size_t i = 0; i < dxData.size(); ++i) {
    for(size_t j = 0; j < dyData.size(); ++j) {
      RealType x,y,z;
      x = dxData[i];
      y = dyData[j];
      z = dzData[i * dyData.size() + j];
      while(x < intervals[indexi][indexj].xleft) {
        indexi--;
      }
      std::cout << (x > intervals[indexi][indexj].xright);
      while(x > intervals[indexi][indexj].xright) {
        indexi++;
      }
      while(y < intervals[indexi][indexj].yleft) {
        indexj--;
      }
      while(y > intervals[indexi][indexj].yright) {
        indexj++;
      }
      intervals[indexi][indexj].b += z;
    }
  }
  indexi = 0;
  indexj = 0;
}

RealType Scenarios::TsunamiScenario::getWaterHeight(RealType x, RealType y) const {
  while(x < intervals[indexi][indexj].xleft) {
    indexi--;
  }
  while(x > intervals[indexi][indexj].xright) {
    indexi++;
  }
  while(y < intervals[indexi][indexj].yleft) {
    indexj--;
  }
  while(y > intervals[indexi][indexj].yright) {
    indexj++;
  }
  return intervals[indexi][indexj].h;
}

RealType Scenarios::TsunamiScenario::getBathymetry([[maybe_unused]] RealType x, [[maybe_unused]] RealType y)
 const {
  while(x < intervals[indexi][indexj].xleft) {
    indexi--;
  }
  while(x > intervals[indexi][indexj].xright) {
    indexi++;
  }
  while(y < intervals[indexi][indexj].yleft) {
    indexj--;
  }
  while(y > intervals[indexi][indexj].yright) {
    indexj++;
  }
  return intervals[indexi][indexj].b;

}

double Scenarios::TsunamiScenario::getEndSimulationTime() const {
 return double(100);
}

BoundaryType Scenarios::TsunamiScenario::getBoundaryType([[maybe_unused]] BoundaryEdge edge) const {
 return BoundaryType::Outflow;
}

RealType Scenarios::TsunamiScenario::getBoundaryPos(BoundaryEdge edge) const {
   if (edge == BoundaryEdge::Left) {
      return RealType(0.0);
   } else if (edge == BoundaryEdge::Right) {
      return RealType(10000.0);
   } else if (edge == BoundaryEdge::Bottom) {
      return RealType(0.0);
   } else {
      return RealType(10000.0);
   }


}
