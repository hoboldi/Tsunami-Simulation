#include "CheckpointScenario.h"

// Constructor that reads the checkpoint-File and sets the values at the corresponding places
Scenarios::CheckpointScenario::CheckpointScenario(const std::string& filename):
  Scenario(),
  timestepStart(Readers::NetCDFReader::readCheckpoint(filename, timePassedAtStart, bathymetries, heights, momentaX, momentaY, boundaries, dx_, dy_, nx_, ny_)),
  // arrays and nx, ny, dx, dy are set in the readCheckpoint function,
  // timestepStart is initialized first because it's declared before the other variables
  bathymetry_(nx_, ny_, bathymetries),
  height_(nx_, ny_, heights),
  momentaX_(nx_, ny_, momentaX),
  momentaY_(nx_, ny_, momentaY) {
  setBoundaryType(boundaries);
}

RealType Scenarios::CheckpointScenario::getWaterHeight(RealType x, RealType y) const {
  if (x < 0 || y < 0 || x >= nx_ * dx_ || y >= ny_ * dy_) {
    return 0;
  }
  int x_scaled = static_cast<int>((x - std::fmod(x, dx_)) / dx_);
  int y_scaled = static_cast<int>((y - std::fmod(y, dy_)) / dy_);
  return height_[x_scaled][y_scaled];
}

RealType Scenarios::CheckpointScenario::getBathymetry(RealType x, RealType y) const {
  if (x < 0 || y < 0 || x >= nx_ * dx_ || y >= ny_ * dy_) {
    return 0;
  }
  int x_scaled = static_cast<int>((x - std::fmod(x, dx_)) / dx_);
  int y_scaled = static_cast<int>((y - std::fmod(y, dy_)) / dy_);
  return bathymetry_[x_scaled][y_scaled];
}

RealType Scenarios::CheckpointScenario::getVelocityU(RealType x, RealType y) const {
  if (x < 0 || y < 0 || x >= nx_ * dx_ || y >= ny_ * dy_) {
    return 0;
  }
  int x_scaled = static_cast<int>((x - std::fmod(x, dx_)) / dx_);
  int y_scaled = static_cast<int>((y - std::fmod(y, dy_)) / dy_);
  if (height_[x_scaled][y_scaled] == 0) {
    return 0;
  }
  return momentaX_[x_scaled][y_scaled] / height_[x_scaled][y_scaled];
}

RealType Scenarios::CheckpointScenario::getVelocityV(RealType x, RealType y) const {
  if (x < 0 || y < 0 || x >= nx_ * dx_ || y >= ny_ * dy_) {
    return 0;
  }
  int x_scaled = static_cast<int>((x - std::fmod(x, dx_)) / dx_);
  int y_scaled = static_cast<int>((y - std::fmod(y, dy_)) / dy_);
  if (height_[x_scaled][y_scaled] == 0) {
    return 0;
  }
  return momentaY_[x_scaled][y_scaled] / height_[x_scaled][y_scaled];
}


BoundaryType Scenarios::CheckpointScenario::getBoundaryTypeForInteger(int value) {
  if (value == 1) {
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
