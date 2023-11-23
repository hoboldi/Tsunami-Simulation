#include "TsunamiScenario.h"
// mega ugly initialization, readFile sets dx_ dy_ nx_ ny_ and returns the data, so all Variables for Float2D are set
Scenarios::TsunamiScenario::TsunamiScenario(const std::string& bathymetry, const std::string& displacement):
  bathymetry_(nx_ / dx_, ny_ / dy_, Readers::NetCDFReader::readFile(bathymetry, dx_, dy_, nx_, ny_)),
  water_height_(bathymetry_, false) {
  RealType dx_displacement;
  RealType dy_displacement;
  int      nx_displacement;
  int      ny_displacement;
  auto     displacement_data = Readers::NetCDFReader::readFile(displacement, dx_displacement, dy_displacement, nx_displacement, ny_displacement);

  int startx = ((nx_ - nx_displacement) / 2) / dx_;
  int endx   = ((nx_ + nx_displacement) / 2) / dx_;
  int starty = ((ny_ - ny_displacement) / 2) / dy_;
  int endy   = ((ny_ + ny_displacement) / 2) / dy_;

  int rowSize = nx_displacement / dx_displacement;
  for (int i = startx; i < endx; ++i) {
    for (int j = starty; j < endy; ++j) {
      bathymetry_[i][j] = bathymetry_[i][j] + displacement_data[(i - startx) * rowSize + (j - starty)];
    }
  }
}

RealType Scenarios::TsunamiScenario::getWaterHeight(RealType x, RealType y) const {
  if (x < 0 || x > nx_ || y < 0 || y > ny_) {
    return 0.0;
  }
  // round x and y to floor of  nearest multiple of dx and dy
  int x_floor = static_cast<int>((x - fmod(x, dx_)) / dx_);
  int y_floor = static_cast<int>((y - fmod(y, dy_)) / dy_);
  return -std::fmin(0.0, water_height_[x_floor][y_floor]);
}

RealType Scenarios::TsunamiScenario::getBathymetry([[maybe_unused]] RealType x, [[maybe_unused]] RealType y) const {
  if (x < 0 || x > nx_ || y < 0 || y > ny_) {
    return 0.0;
  }
  // round x and y to floor of  nearest multiple of dx and dy
  int x_floor = static_cast<int>((x - fmod(x, dx_)) / dx_);
  int y_floor = static_cast<int>((y - fmod(y, dy_)) / dy_);
  return bathymetry_[x_floor][y_floor];
}

double Scenarios::TsunamiScenario::getEndSimulationTime() const { return double(0.1); }

void Scenarios::TsunamiScenario::setEndSimulationTime(double time) {
  assert(time >= 0);
  endSimulationTime = time;
}


BoundaryType Scenarios::TsunamiScenario::getBoundaryType([[maybe_unused]] BoundaryEdge edge) const { return boundaryType; }

void Scenarios::TsunamiScenario::setBoundaryType(int type) {
  assert(type == 0 || type == 1);
  if (type == 0) {
    boundaryType = BoundaryType::Outflow;
  } else if (type == 1) {
    boundaryType = BoundaryType::Wall;
  } else {
    std::cout << "Boundary type not supported" << std::endl;
  }
}
RealType Scenarios::TsunamiScenario::getBoundaryPos(BoundaryEdge edge) const {
  if (edge == BoundaryEdge::Left) {
    return RealType(0.0);
  } else if (edge == BoundaryEdge::Right) {
    return RealType(nx_);
  } else if (edge == BoundaryEdge::Bottom) {
    return RealType(0);
  } else {
    return RealType(ny_);
  }
}
