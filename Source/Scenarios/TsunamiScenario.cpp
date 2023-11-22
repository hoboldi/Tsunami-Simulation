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

  //Initialize Intervals
  for(size_t i = 0; i < bxData.size(); ++i) {
    std::vector<interval> oneGrid;
    oneGrid.resize(byData.size());
    intervals.push_back(oneGrid);
  }

  //Calculate Original Intervals

  // i = 0, j = 0
  intervals[0][0].xleft = -DBL_MAX;
  intervals[0][0].yleft = -DBL_MAX;
  intervals[0][0].xright = bxData[0];
  intervals[0][0].yright = byData[0];
  intervals[0][0].b = bzData[0];
  intervals[0][0].h = -fmin(intervals[0][0].b, 0);

  //First Line and Column
  for(size_t i = 1; i < bxData.size(); ++i) {
    intervals[i - 1][0].xright = (intervals[i - 1][0].xright + bxData[i]) * 0.5;

    intervals[i][0].xleft = intervals[i - 1][0].xright;
    intervals[i][0].yleft = -DBL_MAX;
    intervals[i][0].xright = bxData[i];
    intervals[i][0].yright = byData[0];
    intervals[i][0].b = bzData[i * byData.size()];
    intervals[i][0].h = -fmin(intervals[i][0].b, 0);
  }

  for(size_t j = 1; j < byData.size(); ++j) {
    intervals[0][j - 1].yright = (intervals[0][j - 1].yright + byData[j]) * 0.5;

    intervals[0][j].xleft = -DBL_MAX;
    intervals[0][j].yleft = intervals[0][j - 1].yright;
    intervals[0][j].xright = bxData[0];
    intervals[0][j].yright = byData[j];
    intervals[0][j].b = bzData[j];
    intervals[0][j].h = -fmin(intervals[0][j].b, 0);
  }


  for(size_t i = 1; i < bxData.size(); ++i) {
    for(size_t j = 1; j < byData.size(); ++j) {
      intervals[i - 1][j].xright = (intervals[i - 1][j].xright + bxData[i]) * 0.5;
      intervals[i][j - 1].yright = (intervals[i][j - 1].yright + byData[j]) * 0.5;

      intervals[i][j].xleft = intervals[i - 1][j].xright;
      intervals[i][j].yleft = intervals[i][j - 1].yright;
      intervals[i][j].xright = i != bxData.size() - 1 ? bxData[i] : DBL_MAX;
      intervals[i][j].yright = j != byData.size() - 1 ? byData[j] : DBL_MAX;
      intervals[i][j].b = bzData[i * byData.size() + j];
      intervals[i][j].h = -fmin(intervals[0][j].b, 0);
    }
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
      return RealType(-500.0);
   } else if (edge == BoundaryEdge::Right) {
      return RealType(500.0);
   } else if (edge == BoundaryEdge::Bottom) {
      return RealType(-500.0);
   } else {
      return RealType(500.0);
   }


}
