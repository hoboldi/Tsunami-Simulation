#include "WorldScenario.h"

#include <cfloat>

unsigned long indexi = 0;
unsigned long indexj = 0;
double        offsetx;
double        offsety;
double        sizex;
double        sizey;


namespace Scenarios {
  std::vector<std::vector<interval>> intervals;
  std::vector<std::vector<interval>> getInterval() { return intervals; }
} // namespace Scenarios

void Scenarios::WorldScenario::PreEarthquake::readWorld(std::string bathymetry) const {
  // Read Bathymetry
  int bncid, bvarid;
  int retval;

  retval = nc_open(bathymetry.c_str(), NC_NOWRITE, &bncid);
  assert(retval == NC_NOERR);

  // Get the variables
  int    bx_dimid, by_dimid;
  int    bx_varid, by_varid, bz_varid;
  size_t bxlen, bylen;

  // Get dimension ids
  retval = nc_inq_dimid(bncid, "x", &bx_dimid);
  assert(retval == NC_NOERR);
  retval = nc_inq_dimid(bncid, "y", &by_dimid);
  assert(retval == NC_NOERR);

  // Get variable ids
  retval = nc_inq_varid(bncid, "x", &bx_varid);
  assert(retval == NC_NOERR);
  retval = nc_inq_varid(bncid, "y", &by_varid);
  assert(retval == NC_NOERR);
  retval = nc_inq_varid(bncid, "z", &bz_varid);
  assert(retval == NC_NOERR);

  // Get dimension lengths
  retval = nc_inq_dimlen(bncid, bx_dimid, &bxlen);
  assert(retval == NC_NOERR);
  retval = nc_inq_dimlen(bncid, by_dimid, &bylen);
  assert(retval == NC_NOERR);

  double bx_data[bxlen];
  double by_data[bylen];
  auto*  bz_data = new double[bxlen * bylen];

  retval = nc_get_var_double(bncid, bx_varid, bx_data);
  assert(retval == NC_NOERR);
  retval = nc_get_var_double(bncid, by_varid, by_data);
  assert(retval == NC_NOERR);
  retval = nc_get_var_double(bncid, bz_varid, bz_data);
  assert(retval == NC_NOERR);

  offsetx = bx_data[0];
  offsety = by_data[0];
  if (bx_data[bxlen - 1] < 0) {
    sizex = fabs(bx_data[0]) - fabs(bx_data[bxlen - 1]);
  } else {
    if (bx_data[0] < 0) {
      sizex = fabs(bx_data[bxlen - 1]) + fabs(bx_data[0]);
    } else {
      sizex = bx_data[bxlen - 1] - bx_data[0];
    }
  }
  if (by_data[bylen - 1] < 0) {
    sizey = fabs(by_data[0]) - fabs(by_data[bylen - 1]);
  } else {
    if (by_data[0] < 0) {
      sizey = fabs(by_data[bylen - 1]) + fabs(by_data[0]);
    } else {
      sizey = by_data[bylen - 1] - by_data[0];
    }
  }

  std::vector<double> bxData(bxlen);
  std::vector<double> byData(bylen);
  std::vector<double> bzData(bxlen * bylen);


  for (size_t i = 0; i < bxlen; i++) {
    bxData[i] = bx_data[i];
  }
  for (size_t i = 0; i < bylen; i++) {
    byData[i] = by_data[i];
  }
  for (size_t i = 0; i < bxlen * bylen; i++) {
    bzData[i] = bz_data[i];
  }


  for (size_t i = 0; i < bxlen - 1; i++) {
    assert(bxData[i] < bxData[i + 1]);
  }
  for (size_t i = 0; i < bylen - 1; i++) {
    assert(byData[i] < byData[i + 1]);
  }

  // Initialize Intervals
  for (size_t i = 0; i < bxData.size(); ++i) {
    std::vector<interval> oneGrid;
    oneGrid.resize(byData.size());
    intervals.push_back(oneGrid);
  }

  // Calculate Original Intervals

  // i = 0, j = 0
  intervals[0][0].xleft  = -DBL_MAX;
  intervals[0][0].yleft  = -DBL_MAX;
  intervals[0][0].xright = bxData[0];
  intervals[0][0].yright = byData[0];
  intervals[0][0].b      = bzData[0];
  intervals[0][0].h      = -fmin(intervals[0][0].b, 0);

  // First Line and Column
  for (size_t i = 1; i < bxData.size(); ++i) {
    intervals[i - 1][0].xright = (intervals[i - 1][0].xright + bxData[i]) * 0.5;

    intervals[i][0].xleft  = intervals[i - 1][0].xright;
    intervals[i][0].yleft  = -DBL_MAX;
    intervals[i][0].xright = i != bxData.size() - 1 ? bxData[i] : DBL_MAX;
    intervals[i][0].yright = byData[0];
    intervals[i][0].b      = bzData[i];
    intervals[i][0].h      = -fmin(intervals[i][0].b, 0);
  }

  for (size_t j = 1; j < byData.size(); ++j) {
    intervals[0][j - 1].yright = (intervals[0][j - 1].yright + byData[j]) * 0.5;

    intervals[0][j].xleft  = -DBL_MAX;
    intervals[0][j].yleft  = intervals[0][j - 1].yright;
    intervals[0][j].xright = bxData[0];
    intervals[0][j].yright = j != byData.size() - 1 ? byData[j] : DBL_MAX;
    intervals[0][j].b      = bzData[j * bxData.size()];
    intervals[0][j].h      = -fmin(intervals[0][j].b, 0);
  }


  for (size_t i = 1; i < bxData.size(); ++i) {
    for (size_t j = 1; j < byData.size(); ++j) {
      intervals[i - 1][j].xright = (intervals[i - 1][j].xright + bxData[i]) * 0.5;
      intervals[i][j - 1].yright = (intervals[i][j - 1].yright + byData[j]) * 0.5;

      intervals[i][j].xleft  = intervals[i - 1][j].xright;
      intervals[i][j].yleft  = intervals[i][j - 1].yright;
      intervals[i][j].xright = i != bxData.size() - 1 ? bxData[i] : DBL_MAX;
      intervals[i][j].yright = j != byData.size() - 1 ? byData[j] : DBL_MAX;
      intervals[i][j].b      = bzData[j * bxData.size() + i];
      intervals[i][j].h      = -fmin(intervals[i][j].b, 0);
    }
  }
  // close the file
  nc_close(bncid);

  indexi = 0;
  indexj = 0;

  delete[] bz_data;
}

RealType Scenarios::WorldScenario::PreEarthquake::getWaterHeight(RealType x, RealType y) const {
  x += offsetx;
  y += offsety;
  while (x < intervals[indexi][indexj].xleft) {
    indexi--;
  }
  while (x > intervals[indexi][indexj].xright) {
    indexi++;
  }
  while (y < intervals[indexi][indexj].yleft) {
    indexj--;
  }
  while (y > intervals[indexi][indexj].yright) {
    indexj++;
  }
  return intervals[indexi][indexj].h;
}

RealType Scenarios::WorldScenario::PreEarthquake::getBathymetry([[maybe_unused]] RealType x, [[maybe_unused]] RealType y) const {
  x += offsetx;
  y += offsety;
  while (x < intervals[indexi][indexj].xleft) {
    indexi--;
  }
  while (x > intervals[indexi][indexj].xright) {
    indexi++;
  }
  while (y < intervals[indexi][indexj].yleft) {
    indexj--;
  }
  while (y > intervals[indexi][indexj].yright) {
    indexj++;
  }
  if (intervals[indexi][indexj].b < 20 && intervals[indexi][indexj].b >= 0) {
    return 20;
  }
  if (intervals[indexi][indexj].b > -20 && intervals[indexi][indexj].b < 0) {
    return -20;
  }

  return intervals[indexi][indexj].b;
}