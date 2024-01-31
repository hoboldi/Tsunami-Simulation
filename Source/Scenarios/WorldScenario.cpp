#pragma once
#include "WorldScenario.h"

#include <cfloat>

unsigned long indexiW = 0;
unsigned long indexjW = 0;
double        offsetxW;
double        offsetyW;
double        sizexW;
double        sizeyW;

namespace Scenarios {
  std::vector<std::vector<interval>> intervalsW;
  std::vector<std::vector<interval>> getIntervalW() { return intervalsW; }
} // namespace Scenarios

Scenarios::WorldScenario::WorldScenario(RealType epicenterX, RealType epicenterY, RealType magnitude, RealType bathymetryInEpicenter)
{
    this->epicenterX = epicenterX;
    this-> epicenterY = epicenterY;
    this->magnitude = magnitude;
}

void Scenarios::WorldScenario::readWorld(std::string bathymetry) const {
  // Read Bathymetry
  int bncid, bvarid;
  int retval;

  retval = nc_open(bathymetry.c_str(), NC_NOWRITE, &bncid);
  assert(retval == NC_NOERR);
  std::cout << "We're here" << std::endl;
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

  offsetxW = bx_data[0];
  offsetyW = by_data[0];
  if (bx_data[bxlen - 1] < 0) {
    sizexW = fabs(bx_data[0]) - fabs(bx_data[bxlen - 1]);
  } else {
    if (bx_data[0] < 0) {
      sizexW = fabs(bx_data[bxlen - 1]) + fabs(bx_data[0]);
    } else {
      sizexW = bx_data[bxlen - 1] - bx_data[0];
    }
  }
  if (by_data[bylen - 1] < 0) {
    sizeyW = fabs(by_data[0]) - fabs(by_data[bylen - 1]);
  } else {
    if (by_data[0] < 0) {
      sizeyW = fabs(by_data[bylen - 1]) + fabs(by_data[0]);
    } else {
      sizeyW = by_data[bylen - 1] - by_data[0];
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

  // Initialize intervalsW
  for (size_t i = 0; i < bxData.size(); ++i) {
    std::vector<interval> oneGrid;
    oneGrid.resize(byData.size());
    intervalsW.push_back(oneGrid);
  }

  // Calculate Original intervalsW

  // i = 0, j = 0
  intervalsW[0][0].xleft  = -DBL_MAX;
  intervalsW[0][0].yleft  = -DBL_MAX;
  intervalsW[0][0].xright = bxData[0];
  intervalsW[0][0].yright = byData[0];
  intervalsW[0][0].b      = bzData[0];
  RealType bathy = intervalsW[0][0].b;
  if(bathy < 20 && bathy >= 0){
    bathy = 20;
  }else if(bathy > -20 && bathy < 0){
    bathy = -20;
  }
  intervalsW[0][0].h      = -fmin(bathy, 0);

  // First Line and Column
  for (size_t i = 1; i < bxData.size(); ++i) {
    intervalsW[i - 1][0].xright = (intervalsW[i - 1][0].xright + bxData[i]) * 0.5;

    intervalsW[i][0].xleft  = intervalsW[i - 1][0].xright;
    intervalsW[i][0].yleft  = -DBL_MAX;
    intervalsW[i][0].xright = i != bxData.size() - 1 ? bxData[i] : DBL_MAX;
    intervalsW[i][0].yright = byData[0];
    intervalsW[i][0].b      = bzData[i];
    bathy = intervalsW[i][0].b;
    if(bathy < 20 && bathy >= 0){
      bathy = 20;
    }else if(bathy > -20 && bathy < 0){
      bathy = -20;
    }
    intervalsW[i][0].h      = -fmin(bathy, 0);
  }

  for (size_t j = 1; j < byData.size(); ++j) {
    intervalsW[0][j - 1].yright = (intervalsW[0][j - 1].yright + byData[j]) * 0.5;

    intervalsW[0][j].xleft  = -DBL_MAX;
    intervalsW[0][j].yleft  = intervalsW[0][j - 1].yright;
    intervalsW[0][j].xright = bxData[0];
    intervalsW[0][j].yright = j != byData.size() - 1 ? byData[j] : DBL_MAX;
    intervalsW[0][j].b      = bzData[j * bxData.size()];
    bathy = intervalsW[0][j].b;
    if(bathy < 20 && bathy >= 0){
      bathy = 20;
    }else if(bathy > -20 && bathy < 0){
      bathy = -20;
    }
    intervalsW[0][j].h      = -fmin(bathy, 0);
  }


  for (size_t i = 1; i < bxData.size(); ++i) {
    for (size_t j = 1; j < byData.size(); ++j) {
      intervalsW[i - 1][j].xright = (intervalsW[i - 1][j].xright + bxData[i]) * 0.5;
      intervalsW[i][j - 1].yright = (intervalsW[i][j - 1].yright + byData[j]) * 0.5;

      intervalsW[i][j].xleft  = intervalsW[i - 1][j].xright;
      intervalsW[i][j].yleft  = intervalsW[i][j - 1].yright;
      intervalsW[i][j].xright = i != bxData.size() - 1 ? bxData[i] : DBL_MAX;
      intervalsW[i][j].yright = j != byData.size() - 1 ? byData[j] : DBL_MAX;
      intervalsW[i][j].b      = bzData[j * bxData.size() + i];
      bathy = intervalsW[i][j].b;
      if(bathy < 20 && bathy >= 0){
        bathy = 20;
      }else if(bathy > -20 && bathy < 0){
        bathy = -20;
      }
      intervalsW[i][j].h      = -fmin(bathy, 0);
    }
  }
  // close the file
  nc_close(bncid);
  indexiW = 0;
  indexjW = 0;

  delete[] bz_data;
  std::cout << "Just a test" << std::endl;
}

RealType Scenarios::WorldScenario::getStartingWaveHeight() const
{
    RealType maxHeight = getMaxWaveHeight();
    // We use the equation found here: https://en.wikipedia.org/wiki/Green%27s_law#:~:text=In%20fluid%20dynamics%2C%20Green's%20law,gradually%20varying%20depth%20and%20width.
    // H1 * foruth root of h1 = H2 * fourth root of h2
    // H1 and H2 are the wave heights in 2 places, h1 and h2 being the corresponding water heights
    // Since we only try and calculate until a water height of 50m, we use that as the value for h1, and h2 being tha depth in the epicenter-cell
    RealType startingHeight = (maxHeight * std::pow(50, 1.0/4)) / std::pow(heightInEpicenter, 1.0/4);
}

RealType Scenarios::WorldScenario::getMaxWaveHeight() const
{
    if (magnitude < 6.51)
    {
        return 0;
    }
    else
    {
        return (7,6875 * magnitude) - 50,0417;
    }
}

RealType Scenarios::WorldScenario::getWaterHeight(RealType x, RealType y) const {
  x += offsetxW;
  y += offsetyW;
  while (x < intervalsW[indexiW][indexjW].xleft) {
    indexiW--;
  }
  while (x > intervalsW[indexiW][indexjW].xright) {
    indexiW++;
  }
  while (y < intervalsW[indexiW][indexjW].yleft) {
    indexjW--;
  }
  while (y > intervalsW[indexiW][indexjW].yright) {
    indexjW++;
  }
  return intervalsW[indexiW][indexjW].h;
}


RealType Scenarios::WorldScenario::getBathymetry([[maybe_unused]] RealType x, [[maybe_unused]] RealType y) const {
  x += offsetxW;
  y += offsetyW;
  while (x < intervalsW[indexiW][indexjW].xleft) {
    indexiW--;
  }
  while (x > intervalsW[indexiW][indexjW].xright) {
    indexiW++;
  }
  while (y < intervalsW[indexiW][indexjW].yleft) {
    indexjW--;
  }
  while (y > intervalsW[indexiW][indexjW].yright) {
    indexjW++;
  }
  if (intervalsW[indexiW][indexjW].b < 20 && intervalsW[indexiW][indexjW].b >= 0) {
    return 20;
  }
  if (intervalsW[indexiW][indexjW].b > -20 && intervalsW[indexiW][indexjW].b < 0) {
    return -20;
  }

  return intervalsW[indexiW][indexjW].b;
}

void Scenarios::WorldScenario::adjustDomain(RealType bottomLeft, RealType topRight, bool isOverEdge) {

}

double Scenarios::WorldScenario::getEndSimulationTime() const { return endSimulationTime; }

void Scenarios::WorldScenario::setEndSimulationTime(double time) {
  assert(time >= 0);
  endSimulationTime = time;
}

RealType Scenarios::WorldScenario::getBoundaryPos(BoundaryEdge edge) const {
  if (edge == BoundaryEdge::Left) {
    return RealType(0.0);
  } else if (edge == BoundaryEdge::Right) {
    return sizexW;
  } else if (edge == BoundaryEdge::Bottom) {
    return RealType(0.0);
  } else {
    return sizeyW;
  }
}
