//
// Created by simon on 1/26/24.
//

#include "FileScenario.h"

#include <cmath>
Scenarios::FileScenario::FileScenario(const std::string& bathymetry, int numCellsX, int numCellsY, int offsetX, RealType epicenterX, RealType epicenterY, RealType magnitude):
  reader_(bathymetry),
  offsetX_(offsetX){
  xDim = static_cast<double>(reader_.getYDim());
  yDim = static_cast<double>(reader_.getXDim());
  dx_ = xDim / numCellsX;
  dy_ = yDim / numCellsY;
  this->epicenterX = epicenterX;
  this->epicenterY = epicenterY;
  this->magnitude = magnitude;
}

RealType Scenarios::FileScenario::getStartingWaveHeight() const
{
    RealType maxHeight = getMaxWaveHeight();
    // We use the equation found here: https://en.wikipedia.org/wiki/Green%27s_law#:~:text=In%20fluid%20dynamics%2C%20Green's%20law,gradually%20varying%20depth%20and%20width.
    // H1 * foruth root of h1 = H2 * fourth root of h2
    // H1 and H2 are the wave heights in 2 places, h1 and h2 being the corresponding water heights
    // Since we only try and calculate until a water height of 50m, we use that as the value for h1, and h2 being tha depth in the epicenter-cell
    RealType startingHeight = (maxHeight * std::pow(50, 1.0/4)) / std::pow(-1 * getBathymetry(epicenterX * dx_, epicenterY * dy_), 1.0/4); //TODO
    //print wave height
    std::cout << "Starting: " << startingHeight << std::endl;
    return startingHeight;
}

RealType Scenarios::FileScenario::getMaxWaveHeight() const
{
    if (magnitude < 6.51)
    {
        return 0;
    }
    else
    {
        return (7.6875 * magnitude) - 50.0417;
    }
}

inline RealType Scenarios::FileScenario::getBathymetry(const RealType x, const RealType y) const {
    if(x < 0 || x > xDim || y < 0 || y > yDim){
        return 0;
    }
  int    y_index = (offsetX_ + x);
  y_index %= static_cast<int>(xDim);
  int    x_index = (y );
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
  //print values x, y and epicenter
  //std::cout << "x: " << x << " y: " << y << " epicenterX: " << epicenterX  << " epicenterY: " << epicenterY << std::endl;


  int   y_index = (offsetX_ + x);
  y_index %= static_cast<int>(xDim);
  int   x_index = (y);
  double val     = reader_.readUnbuffered(x_index, y_index);

  RealType result = 0;
  if (val < 20 && val >= 0) {
    result = -20;
  } else if (val >= -20 && val < 0) {
    result = 20;
  } else {
    result = -fmin(val, 0);
  }
  //add tsunami wave
  if (abs(x - epicenterX * dx_) < 5 * dx_ && abs(y - epicenterY * dy_) < 5 * dy_)
  {

    //print test
    std::cout << "Starting: " << getStartingWaveHeight() << std::endl;
    std::cout << "Bathymetrie: " << getBathymetry(x, y) << std::endl;

    result += getStartingWaveHeight() * 1/(1 + ((abs(x - epicenterX * dx_)/dy_) * (abs(y - epicenterY * dy_)/dx_)));
  }
  return result;
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
