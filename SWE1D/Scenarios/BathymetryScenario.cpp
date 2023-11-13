#include "BathymetryScenario.h"
#include <cassert>

 constexpr int cityPosition = 75;

Scenarios::BathymetryScenario::BathymetryScenario(unsigned int size):
  size_(size) {}

RealType Scenarios::BathymetryScenario::getCellSize() const { return RealType(1000) / size_; }

unsigned int Scenarios::BathymetryScenario::getHeight(unsigned int pos) const {
  if (pos < size_ * (0.45)) {
    return 20;
  }
  if(pos >= size_ * (0.45) && pos <= size_ * (0.5)){
      return 16;
  }
    if(pos > size_ * (0.5) && pos <= size_ * (0.55)){
      return 6;
  }
  return 10;
}

int Scenarios::BathymetryScenario::getMomentum(unsigned int pos) const {
  //At the start there is no momentum.
  if (pos <= size_ / 2) {
    return 0;
  }
  return 0.7;
}

int Scenarios::BathymetryScenario::getBathymetry(unsigned int pos) const {
  if(pos >= size_ * (0.45) && pos <= size_ * (0.55)){
      return -16;
  }
  return -20;
}



