#include "ReducedDimSplittingBlock.h"

#include <chrono>
#include <csignal>
Blocks::ReducedDimSplittingBlock::ReducedDimSplittingBlock(int nx, int ny, RealType dx, RealType dy, std::pair<int, int> startCell, std::pair<int, int> endCell):
  DimensionalSplitting(nx, ny, dx, dy),
  startCell_(startCell),
  endCell_(endCell) {}

void Blocks::ReducedDimSplittingBlock::findSearchArea(Gui::Gui& gui) {
  Tools::Float2D<RealType> heightView(h_, false);

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
  // check in a 3x3 area around the strait line between start and end if bathy is > 0 (that means there is an obstacle in the straight line)
  // if there is an obstacle, we need another search method
  bool   obstacle = false;
  double m        = (endCell_.second - startCell_.second) / (double)(endCell_.first - startCell_.first);
  double b        = startCell_.second - m * startCell_.first;
  int    i        = startCell_.first;
  int    j        = startCell_.second;
  int    obstk    = 0;
  int    obstl    = 0;
  while (i != endCell_.first && j != endCell_.second) {
    obstacle = false;
    // search 3x3 area around i,j
    for (int k = i - 1; k <= i + 1; k++) {
      for (int l = j - 1; l <= j + 1; l++) {

        if (b_[k][l] > 0) {
          heightView[k][l] = -FLT_MAX;
          obstacle         = true;
          obstk            = k;
          obstl            = l;
          break;
        } else {
          heightView[k][l] = FLT_MAX;
        }
      }
      if (obstacle) {
        break;
      }
    }
    gui.update(heightView, 0.0);

    if (obstacle) {
      // check in which direction the obstacle is and go in other direction (like ray bounce)
      // update m and b
      m = -1 / m;
      b = obstl - m * obstk;

      // take a step back
      i -= startCell_.first < endCell_.first ? 1 : -1;
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
