#include "CheckpointScenario.h"

//Constructor that reads the checkpoint-File and sets the values at the corresponding places
Scenarios::CheckpointScenario::CheckpointScenario(const std::string& filename){
    double* bathymetries;
    double* heights;
    double* momentaX;
    double* momentaY;
    boundaries = new int[4];
    
    timestepStart = Readers::NetCDFReader::readCheckpoint(filename, timePassedAtStart, bathymetries, heights, momentaX, momentaY, boundaries*, dx_, dy_, nx_, ny_);
    bathymetry_(nx_ / dx_, ny_ / dy_, bathymetries);
    height_(bathymetry_, heights);
    momentaX_(bathymetry_, momentaX);
    momentaY_(bathymetry_, momentaY);

    int startx = ((nx_ - nx_displacement) / 2) / dx_;
    int endx   = ((nx_ + nx_displacement) / 2) / dx_;
    int starty = ((ny_ - ny_displacement) / 2) / dy_;
    int endy   = ((ny_ + ny_displacement) / 2) / dy_;
}

RealType Scenarios::CheckpointScenario::getWaterHeight(RealType x, RealType y) const {
  return heights_[x][y];
}

RealType Scenarios::CheckpointScenario::getBathymetry(RealType x, RealType y) const {
  return bathymetry_[x][y];
}

RealType Scenarios::CheckpointScenario::getDischargeHu(RealType x, RealType y) const {
  return momentaX_[x][y];
}

RealType Scenarios::CheckpointScenario::getDischargeHv(RealType x, RealType y) const {
  return momentaY_[x][y];
}

BoundaryType Scenarios::CheckpointScenario::getBoundaryType(BoundaryEdge edge) const {
    if (edge == edge == BoundaryEdge::Top) {
        return getBoundaryTypeForInteger(boundaries[0]);
    }
    else if (edge == edge == BoundaryEdge::Right) {
        return getBoundaryTypeForInteger(boundaries[1]);
    }
    else if (edge == edge == BoundaryEdge::Bottom) {
        return getBoundaryTypeForInteger(boundaries[2]);
    }
    else {
        return getBoundaryTypeForInteger(boundaries[3]);
    }
}

int Scenarios::CheckpointScenario::getBoundaryTypeForInteger(int value) const {
    if (value == 0)
    {
        BoundaryType::Outflow
    }
    return BoundaryType::Wall;
}

RealType Scenarios::CheckpointScenario::getBoundaryPos(BoundaryEdge edge) const {
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