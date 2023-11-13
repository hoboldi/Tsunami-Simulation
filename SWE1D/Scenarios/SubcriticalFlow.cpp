#include "SubcriticalFlow.h"

#include <cassert>
#include <cmath>
#include <iostream>

Scenarios::SubcriticalFlow::SubcriticalFlow(unsigned int size):
  size_(size) {}

RealType Scenarios::SubcriticalFlow::getCellSize() const { return RealType(1000) / size_; }

RealType Scenarios::SubcriticalFlow::getHeight(int pos) const {
  //position can't be negative
  RealType height = 0;
  assert(pos >= 0);
  if (pos <= 25 && pos >= 0) {
    height = -1 * getBathymetry(pos);
    return height;
  }
  return 0;
}

RealType Scenarios::SubcriticalFlow::getMomentum(int pos) const {
  //position can't be negative
  assert(pos >= 0);
  RealType momentum = 0;
  //At the start there is no momentum.
  if (pos <= 25 && pos >= 0) {
    momentum = 4.42;
    return 4.42;
  }
  return 0;
}

RealType Scenarios::SubcriticalFlow::getBathymetry(int pos) const {
  //position can't be negative
  assert(pos >= 0);
  RealType bathymetry = -2;
  if (pos < 12 && pos > 8) {
    bathymetry = -1.8 - 0.05*(pos - 10)*(pos - 10);
    return bathymetry;
  }
  return -2;
}

RealType Scenarios::SubcriticalFlow::getFroudNumber(int pos) const
{
  //position can't be negative
  assert(pos >= 0);
  if (pos <= 25)
  {
    RealType h = getHeight(pos);
    // To prevent dividing by 0 in case that ever happens
    if (h == 0)
    {
      return 0;
    }
    RealType u = getMomentum(pos) / h;
    return u / sqrt(g * h);
  }
  return 0;
}

void Scenarios::SubcriticalFlow::getMaximumFroudNumber(RealType* value, RealType* position) const
{
  if (size_ > 0)
  {
    RealType maxFroud = getFroudNumber(0);
    int maxPos = 0;
    for (int i = 1; i < size_; i++)
    {
      if (getFroudNumber(i) > maxFroud)
      {
        maxFroud = getFroudNumber(i);
        maxPos = i;
      }
    }
    *value = maxFroud;
    *position = maxPos;
    std::cout << "MaxFroud: " << maxFroud << " at position: " << maxPos << std::endl;
  }
  else
  {
    *value = -1;
    *position = -1;
    std::cout << "Size smaller than 0, can't copmpute MaxFroud!" << std::endl;
  }
}

double Scenarios::SubcriticalFlow::round_to(RealType value, double precision) const
{
    return std::round(value / precision) * precision;
}