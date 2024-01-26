//
// Created by simon on 1/26/24.
//

#include "FileScenario.h"

#include <cmath>
Scenarios::FileScenario::FileScenario(const std::string& bathymetry, int numCellsX, int numCellsY, int offsetX):
  reader_(bathymetry),
  offsetX_(offsetX){
  xDim = static_cast<double>(reader_.getYDim());
  yDim = static_cast<double>(reader_.getXDim());
  dx_ = xDim / numCellsX;
  dy_ = yDim / numCellsY;
}
inline RealType Scenarios::FileScenario::getBathymetry(const RealType x, const RealType y) const {
  if(x < 0 || x > xDim || y < 0 || y > yDim){
    return 0;
  }

  int    y_index = (offsetX_ + x) + 0.5 * dx_;
  y_index %= static_cast<int>(xDim);
  int    x_index = (y ) + 0.5 * dy_;
  double val     = reader_.readUnbuffered(x_index, y_index);
  if (val < 20 && val >= 0) {
    return 20;
  } else if (val >= -20 && val < 0) {
    return -20;
  } else {
    return val;
  }
}
inline RealType Scenarios::FileScenario::getWaterHeight(const RealType x, const RealType y) const {
  if(x < 0 || x > xDim || y < 0 || y > yDim){
    return 0;
  }
  int   y_index = (offsetX_ + x) + 0.5 * dx_;
  y_index %= static_cast<int>(xDim);
  int   x_index = (y) + 0.5 * dy_;
  double val     = reader_.readUnbuffered(x_index, y_index);

  if (val < 20 && val >= 0) {
    return 0;
  } else if (val >= -20 && val < 0) {
    return -20;
  } else {
    return -fmin(val, 0);
  }
}
RealType Scenarios::FileScenario::getBoundaryPos(const BoundaryEdge edge) const {
  if (edge == BoundaryEdge::Left) {
    return 0;
  } else if (edge == BoundaryEdge::Right) {
    return xDim;
  } else if (edge == BoundaryEdge::Bottom) {
    return 0;
  } else if (edge == BoundaryEdge::Top) {
    return yDim;
  }
}
