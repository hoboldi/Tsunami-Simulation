#include "ReducedDimSplittingBlock.h"

#include <chrono>
#include <csignal>
#include <queue>
#include <unordered_set>
Blocks::ReducedDimSplittingBlock::ReducedDimSplittingBlock(int nx, int ny, RealType dx, RealType dy, std::pair<int, int> startCell, std::pair<int, int> endCell):
  DimensionalSplitting(nx, ny, dx, dy),
  startCell_(startCell),
  endCell_(endCell) {}


struct Cell {
  static std::pair<int, int> goal;
  int   x, y;
  double cost;   // g(n) - cost from start to this node

  Cell(int x, int y, float cost):
    x(x),
    y(y),
    cost(cost){}

  double TotalCost() const {
    // Heuristic: straight-line distance to goal
    return cost * 0.1 + std::sqrt(std::pow(x - goal.first, 2) +std::pow(y - goal.second,2));
  }
};

struct CompareCell {
  bool operator()(const Cell* a, const Cell* b) const { return a->TotalCost() > b->TotalCost(); }
};
std::pair<int, int> Cell::goal;

void Blocks::ReducedDimSplittingBlock::findSearchArea(Gui::Gui& gui) {
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

        // Check for valid cell, no obstacle, and not visited
        if (b_[newX][newY] < 0 && heightView[newX][newY] != FLT_MAX ) {

          float newCost = current->cost + 1; // Assuming uniform cost
          openSet.push(new Cell(newX, newY, newCost));
        }
      }
    }
  }

  // leave a 3 cell margin around the start and end cell
  bottomCorner_.first  = std::max(0, std::min(startCell_.first, endCell_.first) - 3);
  bottomCorner_.second = std::max(0, std::min(startCell_.second, endCell_.second) - 3);
  topCorner_.first     = std::min(nx_ - 1, std::max(startCell_.first, endCell_.first) + 3);
  topCorner_.second    = std::min(ny_ - 1, std::max(startCell_.second, endCell_.second) + 3);
}
void Blocks::ReducedDimSplittingBlock::findSearchArea() {
  // check in a 3x3 area around the strait line between start and end if bathy is > 0 (that means there is an obstacle in the straight line)
  // if there is an obstacle, we need another search method
  bool   obstacle = false;
  double m        = (endCell_.second - startCell_.second) / (endCell_.first - startCell_.first);
  double b        = startCell_.second - m * startCell_.first;
  int    i        = startCell_.first;
  int    j        = startCell_.second;
  while (i != endCell_.first && j != endCell_.second) {
    // search 3x3 area around i,j
    for (int k = i - 1; k <= i + 1; k++) {
      for (int l = j - 1; l <= j + 1; l++) {
        if (b_[k][l] > 0) {
          obstacle = true;
          break;
        }
      }
    }
    // check if start is directly above/below end, if so dont change i
    if (startCell_.second == endCell_.second) {
      j += startCell_.second < endCell_.second ? 1 : -1;
    } else {
      // check if start is before end, if so, go right, else go left
      i += startCell_.first < endCell_.first ? 1 : -1;
      j = static_cast<int>(m * i + b);
    }
  }
  if (obstacle) {
    bottomCorner_ = startCell_;
    topCorner_    = endCell_;
  } else {
    // leave a 3 cell margin around the start and end cell
    bottomCorner_.first  = std::max(0, std::min(startCell_.first, endCell_.first) - 3);
    bottomCorner_.second = std::max(0, std::min(startCell_.second, endCell_.second) - 3);
    topCorner_.first     = std::min(nx_ - 1, std::max(startCell_.first, endCell_.first) + 3);
    topCorner_.second    = std::min(ny_ - 1, std::max(startCell_.second, endCell_.second) + 3);
  }
}
