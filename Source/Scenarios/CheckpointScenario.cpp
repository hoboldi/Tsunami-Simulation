#include "CheckpointScenario.h"

// Constructor that reads the checkpoint-File and sets the values at the corresponding places
Scenarios::CheckpointScenario::CheckpointScenario(const std::string& filename):
  timestepStart(Readers::NetCDFReader::readCheckpoint(filename, timePassedAtStart, bathymetries, heights, momentaX, momentaY, boundaries, dx_, dy_, nx_, ny_)),
  // arrays and nx, ny, dx, dy are set in the readCheckpoint function,
  // timestepStart is initialized first because it's declared before the other variables
  bathymetry_(nx_, ny_, bathymetries),
  height_(nx_, ny_, heights),
  momentaX_(nx_, ny_, momentaX),
  momentaY_(nx_, ny_, momentaY) {}

RealType Scenarios::CheckpointScenario::getWaterHeight(RealType x, RealType y) const {
  if (x < 0 || y < 0 || x >= nx_ *dx_ || y >= ny_ * dy_) {
    return 0;
  }
  int x_scaled = static_cast<int>((x - std::fmod(x, dx_)) / dx_);
  int y_scaled = static_cast<int>((y - std::fmod(y, dy_)) / dy_);
  return height_[x_scaled][y_scaled];
}

RealType Scenarios::CheckpointScenario::getBathymetry(RealType x, RealType y) const {
  if (x < 0 || y < 0 || x >= nx_ *dx_ || y >= ny_ * dy_) {
    return 0;
  }
  int x_scaled = static_cast<int>((x - std::fmod(x, dx_)) / dx_);
  int y_scaled = static_cast<int>((y - std::fmod(y, dy_)) / dy_);
  return bathymetry_[x_scaled][y_scaled];
}

RealType Scenarios::CheckpointScenario::getDischargeHu(RealType x, RealType y) const {
  if (x < 0 || y < 0 || x >= nx_ *dx_ || y >= ny_ * dy_) {
    return 0;
  }
  int x_scaled = static_cast<int>((x - std::fmod(x, dx_)) / dx_);
  int y_scaled = static_cast<int>((y - std::fmod(y, dy_)) / dy_);
  return momentaX_[x_scaled][y_scaled];
}

RealType Scenarios::CheckpointScenario::getDischargeHv(RealType x, RealType y) const {
  if (x < 0 || y < 0 || x >= nx_ *dx_ || y >= ny_ * dy_) {
    return 0;
  }
  int x_scaled = static_cast<int>((x - std::fmod(x, dx_)) / dx_);
  int y_scaled = static_cast<int>((y - std::fmod(y, dy_)) / dy_);
  return momentaY_[x_scaled][y_scaled];
}

BoundaryType Scenarios::CheckpointScenario::getBoundaryType(BoundaryEdge edge) const {
  if (edge == edge == BoundaryEdge::Top) {
    return getBoundaryTypeForInteger(boundaries[0]);
  } else if (edge == edge == BoundaryEdge::Right) {
    return getBoundaryTypeForInteger(boundaries[1]);
  } else if (edge == edge == BoundaryEdge::Bottom) {
    return getBoundaryTypeForInteger(boundaries[2]);
  } else {
    return getBoundaryTypeForInteger(boundaries[3]);
  }
}

BoundaryType Scenarios::CheckpointScenario::getBoundaryTypeForInteger(int value) const {
  if (value == 0) {
    return BoundaryType::Outflow;
  }
  return BoundaryType::Wall;
}

RealType Scenarios::CheckpointScenario::getBoundaryPos(BoundaryEdge edge) const {
  if (edge == BoundaryEdge::Left) {
    return RealType(0.0);
  } else if (edge == BoundaryEdge::Right) {
    return RealType(nx_ * dx_);
  } else if (edge == BoundaryEdge::Bottom) {
    return RealType(0);
  } else {
    return RealType(ny_ * dy_);
  }
}
RealType Scenarios::CheckpointScenario::getStartTime() const { return timestepStart; }
