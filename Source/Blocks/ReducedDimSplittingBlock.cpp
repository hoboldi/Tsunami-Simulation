#include "ReducedDimSplittingBlock.h"

#include <cfloat>
#include <chrono>
#include <csignal>
#include <iostream>
#include <queue>
#include <unordered_set>
Blocks::ReducedDimSplittingBlock::ReducedDimSplittingBlock(int nx, int ny, RealType dx, RealType dy):
  DimensionalSplitting(nx, ny, dx, dy) {}


struct Cell {
  static std::pair<int, int> goal;
  int                        x, y;
  double                     cost; // g(n) - cost from start to this node

  Cell(int x, int y, float cost):
    x(x),
    y(y),
    cost(cost) {}

  double TotalCost() const {
    // Heuristic: straight-line distance to goal
    return cost * 0.1 + std::sqrt(std::pow(x - goal.first, 2) + std::pow(y - goal.second, 2));
  }
};

struct CompareCell {
  bool operator()(const Cell* a, const Cell* b) const { return a->TotalCost() > b->TotalCost(); }
};
std::pair<int, int> Cell::goal;
#ifdef ENABLE_GUI
void Blocks::ReducedDimSplittingBlock::findSearchArea(Gui::Gui& gui) {
  // if the x-distance between start and end cell is bigger than nx/2, shift the data and get a pacific focused map
  if (abs(startCell_.first - endCell_.first) > nx_ / 2) {
    shiftData();
    startCell_.first = (startCell_.first + nx_ / 2) % nx_;
    endCell_.first   = (endCell_.first + nx_ / 2) % nx_;
    gui.setBathymetry(b_);
    gui.update(h_, 0.0);
  }

  Tools::Float2D<RealType> heightView(h_, false);
  Cell::goal = endCell_;

  // change region around start and end cell to -FLT_MAX
  for (int i = startCell_.first - 3; i <= startCell_.first + 3; i++) {
    for (int j = startCell_.second - 3; j <= startCell_.second + 3; j++) {
      heightView[i][j] = -FLT_MAX;
    }
  }

  for (int i = endCell_.first - 3; i <= endCell_.first + 3; i++) {
    for (int j = endCell_.second - 3; j <= endCell_.second + 3; j++) {
      heightView[i][j] = -FLT_MAX;
    }
  }
  std::priority_queue<Cell*, std::vector<Cell*>, CompareCell> openSet;

  // Initialize bounds
  int minX = startCell_.first, maxX = startCell_.first;
  int minY = startCell_.second, maxY = startCell_.second;

  openSet.push(new Cell(startCell_.first, startCell_.second, 0));
  size_t iterations = 0;
  while (!openSet.empty()) {
    Cell* current = openSet.top();
    openSet.pop();

    // Update bounds
    minX = std::min(minX, current->x);
    maxX = std::max(maxX, current->x);
    minY = std::min(minY, current->y);
    maxY = std::max(maxY, current->y);

    // Mark as visited
    heightView[current->x][current->y] = FLT_MAX;
    // update gui every 20 iterations
    if (iterations % 20 == 0) {
      gui.update(heightView, 0.0);
    }

    // Check if we have reached the goal
    if (current->x == endCell_.first && current->y == endCell_.second) {
      break;
    }

    // Explore neighbors
    for (int dx = -1; dx <= 1; ++dx) {
      for (int dy = -1; dy <= 1; ++dy) {
        if (dx == 0 && dy == 0)
          continue; // Skip the current cell

        int newX = current->x + dx;
        int newY = current->y + dy;
        if (newX < 0 || newX >= nx_ || newY < 0 || newY >= ny_)
          continue; // Skip out of bounds cells

        // Check for valid cell, no obstacle, and not visited
        if (b_[newX][newY] < 0 && heightView[newX][newY] != FLT_MAX) {

          float newCost = current->cost + 1; // Assuming uniform cost
          openSet.push(new Cell(newX, newY, newCost));
        }
      }
    }
  }

  // calculate offset depending on the size of the search area (the bigger the search area, the smaller the offset)
  int offset = 40 - std::log(std::max(maxX - minX, maxY - minY));
  std::cout << "offset: " << offset << std::endl;

  // Set search area
  bottomCorner_.first  = std::max(1, minX - offset);
  bottomCorner_.second = std::max(1, minY - offset);
  topCorner_.first     = std::min(nx_ - 1, maxX + offset);
  topCorner_.second    = std::min(ny_ - 1, maxY + offset);
}
#endif
void Blocks::ReducedDimSplittingBlock::findSearchArea() {
  // if the x-distance between start and end cell is bigger than nx/2, shift the data and get a pacific focused map
  if (abs(startCell_.first - endCell_.first) > nx_ / 2) {
    shiftData();
    startCell_.first = (startCell_.first + nx_ / 2) % nx_;
    endCell_.first   = (endCell_.first + nx_ / 2) % nx_;
  }

  Tools::Float2D<RealType> heightView(h_, false);
  Cell::goal = endCell_;

  // change region around start and end cell to -FLT_MAX
  for (int i = startCell_.first - 3; i <= startCell_.first + 3; i++) {
    for (int j = startCell_.second - 3; j <= startCell_.second + 3; j++) {
      heightView[i][j] = -FLT_MAX;
    }
  }

  for (int i = endCell_.first - 3; i <= endCell_.first + 3; i++) {
    for (int j = endCell_.second - 3; j <= endCell_.second + 3; j++) {
      heightView[i][j] = -FLT_MAX;
    }
  }
  std::priority_queue<Cell*, std::vector<Cell*>, CompareCell> openSet;

  // Initialize bounds
  int minX = startCell_.first, maxX = startCell_.first;
  int minY = startCell_.second, maxY = startCell_.second;

  openSet.push(new Cell(startCell_.first, startCell_.second, 0));
  size_t iterations = 0;
  while (!openSet.empty()) {
    Cell* current = openSet.top();
    openSet.pop();

    // Update bounds
    minX = std::min(minX, current->x);
    maxX = std::max(maxX, current->x);
    minY = std::min(minY, current->y);
    maxY = std::max(maxY, current->y);

    // Mark as visited
    heightView[current->x][current->y] = FLT_MAX;

    // Check if we have reached the goal
    if (current->x == endCell_.first && current->y == endCell_.second) {
      break;
    }

    // Explore neighbors
    for (int dx = -1; dx <= 1; ++dx) {
      for (int dy = -1; dy <= 1; ++dy) {
        if (dx == 0 && dy == 0)
          continue; // Skip the current cell

        int newX = current->x + dx;
        int newY = current->y + dy;

        // Check for valid cell, no obstacle, and not visited
        if (b_[newX][newY] < 0 && heightView[newX][newY] != FLT_MAX) {

          float newCost = current->cost + 1; // Assuming uniform cost
          openSet.push(new Cell(newX, newY, newCost));
        }
      }
    }
  }

  // calculate offset depending on the size of the search area (the bigger the search area, the smaller the offset)
  int offset = 40 - std::log(std::max(maxX - minX, maxY - minY));
  std::cout << "offset: " << offset << std::endl;

  // Set search area
  bottomCorner_.first  = std::max(1, minX - offset);
  bottomCorner_.second = std::max(1, minY - offset);
  topCorner_.first     = std::min(nx_ - 1, maxX + offset);
  topCorner_.second    = std::min(ny_ - 1, maxY + offset);
}
void Blocks::ReducedDimSplittingBlock::computeNumericalFluxes() {
  RealType maxWaveSpeedX{0.0};
  RealType maxWaveSpeedY{0.0};

  /** Calculate the net-updates for the x-stride by iterating over the cells on the x-stride
   * Cells on the boundary are ghost cells and are not updated, but one net update is needed for the neighbouring cell.
   * Layout
   * Q0,4   Q1,4   Q2,4   Q3,4   Q4,4
   * Q0,3 | Q1,3 | Q2,3 | Q3,3 | Q4,3
   * Q0,2 | Q1,2 | Q2,2 | Q3,2 | Q4,2
   * Q0,1 | Q1,1 | Q2,1 | Q3,1 | Q4,1  Qi,j
   * Q0,0   Q1,0   Q2,0   Q3,0   Q4,0  updates = |
   */

#if defined(ENABLE_OPENMP)
#pragma omp parallel for reduction(max : maxWaveSpeedX), schedule(dynamic)
#endif
  for (int i = bottomCorner_.first; i <= topCorner_.first; ++i) {
    for (int j = bottomCorner_.second; j <= topCorner_.second; ++j) {
      RealType maxEdgeSpeed{0.0};

      fWaveSolver_.computeNetUpdates(
        h_[i][j],
        h_[i + 1][j],
        hu_[i][j],
        hu_[i + 1][j],
        b_[i][j],
        b_[i + 1][j],
        hNetUpdatesXLeft_[i][j],
        hNetUpdatesXRight_[i][j],
        huNetUpdatesXLeft_[i][j],
        huNetUpdatesXRight_[i][j],
        maxEdgeSpeed
      );

#if defined(ENABLE_OPENMP)
      maxWaveSpeedX = std::max(maxWaveSpeedX, maxEdgeSpeed);
#else
      // Update the maximum wave speed
      maxWaveSpeedX = std::max(maxWaveSpeedX, maxEdgeSpeed);
#endif
    }
  }

  /** Calculate the net-updates for the y-stride by iterating over the cells on the y-stride
   * Cells on the boundary are ghost cells and are not updated, but one net update is needed for the neighbouring cell.
   * Layout
   * Q0,4 Q1,4 Q2,4 Q3,4 Q4,4
   *      --------------
   * Q0,3 Q1,3 Q2,3 Q3,3 Q4,3
   *      --------------
   * Q0,2 Q1,2 Q2,2 Q3,2 Q4,2
   *      --------------
   * Q0,1 Q1,1 Q2,1 Q3,1 Q4,1
   *      --------------       Qi,j
   * Q0,0 Q1,0 Q2,0 Q3,0 Q4,0  updates = --------------
   */

#if defined(ENABLE_OPENMP)
#pragma omp parallel for reduction(max : maxWaveSpeedY), schedule(dynamic)
#endif
  for (int i = bottomCorner_.first; i < topCorner_.first; ++i) {
    for (int j = bottomCorner_.second; j <= topCorner_.second; ++j) {
      RealType maxEdgeSpeed{0.0};

      fWaveSolver_.computeNetUpdates(
        h_[i][j],
        h_[i][j + 1],
        hv_[i][j],
        hv_[i][j + 1],
        b_[i][j],
        b_[i][j + 1],
        hNetUpdatesYLeft_[i][j],
        hNetUpdatesYRight_[i][j],
        hvNetUpdatesYLeft_[i][j],
        hvNetUpdatesYRight_[i][j],
        maxEdgeSpeed
      );

#if defined(ENABLE_OPENMP)
      maxWaveSpeedY = std::max(maxWaveSpeedY, maxEdgeSpeed);
#else
      // Update the maximum wave speed
      maxWaveSpeedY = std::max(maxWaveSpeedY, maxEdgeSpeed);
#endif
    }
  }


  // Compute the time step width
  maxTimeStep_ = dx_ / maxWaveSpeedX;
  maxTimeStep_ = std::min(maxTimeStep_, dy_ / maxWaveSpeedY);

  // Reduce maximum time step size by "safety factor"
  maxTimeStep_ *= RealType(0.4); // CFL-number = 0.5

  // Debug only check if CFL condition is satisfied for the y-sweep (dt < dy /  (2* maxWaveSpeed))
#ifndef NDEBUG
  if (maxTimeStep_ >= ((dy_ / maxWaveSpeedY) * 0.5)) {
    std::fprintf(stderr, "Warning: CFL condition not satisfied for y-sweep! dt = %f >= %f\n", maxTimeStep_, 0.5 * (dy_ / maxWaveSpeedY));
  }
#endif
}
void Blocks::ReducedDimSplittingBlock::updateUnknowns(RealType dt) {
#if defined(ENABLE_OPENMP)
#pragma omp parallel for schedule(dynamic)
#endif
  for (int i = bottomCorner_.first; i <= topCorner_.first; ++i) {
    for (int j = bottomCorner_.second; j < topCorner_.second; ++j) {
      h_[i][j] -= dt / dx_ * (hNetUpdatesXRight_[i - 1][j] + hNetUpdatesXLeft_[i][j]);
      hu_[i][j] -= dt / dx_ * (huNetUpdatesXRight_[i - 1][j] + huNetUpdatesXLeft_[i][j]);
    }
  }

#if defined(ENABLE_OPENMP)
#pragma omp parallel for schedule(dynamic)
#endif
  for (int i = bottomCorner_.first; i < topCorner_.first; ++i) {
    for (int j = bottomCorner_.second; j <= topCorner_.second; ++j) {
      h_[i][j] -= dt / dy_ * (hNetUpdatesYRight_[i][j - 1] + hNetUpdatesYLeft_[i][j]);
      hv_[i][j] -= dt / dy_ * (hvNetUpdatesYRight_[i][j - 1] + hvNetUpdatesYLeft_[i][j]);
    }
  }
}
void Blocks::ReducedDimSplittingBlock::setStartCell(std::pair<int, int> startCell) { startCell_ = startCell; }

void Blocks::ReducedDimSplittingBlock::setEndCell(std::pair<int, int> endCell) { endCell_ = endCell; }
void Blocks::ReducedDimSplittingBlock::shiftData() {
  // shift bathymetry half of nx to the right and roll the rest to the left
  for (int j = 0; j < ny_; ++j) {
    b_[0][j] = b_[nx_ / 2][j];
    b_[nx_ / 2][j] = -1000;
  }
  for (int i = 1; i < nx_ / 2; i++) {
    for (int j = 0; j < ny_; ++j) {
      double tmp         = b_[i][j];
      b_[i][j]           = b_[i + nx_ / 2][j];
      b_[i + nx_ / 2][j] = tmp;
    }
  }

  // shift h_ half of nx to the right and roll the rest to the left
  for (int j = 0; j < ny_; ++j) {
    h_[0][j] = h_[nx_ / 2][j];
    h_[nx_ / 2][j] = 1000;
  }
  for (int i = 1; i < nx_ / 2; i++) {
    for (int j = 0; j < ny_; ++j) {
      double tmp         = h_[i][j];
      h_[i][j]           = h_[i + nx_ / 2][j];
      h_[i + nx_ / 2][j] = tmp;
    }
  }
}

