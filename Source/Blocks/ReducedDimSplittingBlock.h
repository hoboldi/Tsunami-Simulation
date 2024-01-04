#pragma once
#include "DimensionalSplitting.h"
#include "Solvers/FWaveSolver.h"

namespace Blocks {
  class ReducedDimSplittingBlock : public DimensionalSplitting {
  public:
    ReducedDimSplittingBlock(int nx, int ny, RealType dx, RealType dy, std::pair<int, int> startCell, std::pair<int, int> endCell);
    ~ReducedDimSplittingBlock() override = default;

  private:
    // pair of coordinates of the startcell for the search (epicenter)
    std::pair<int, int> startCell_;
    // pair of coordinates of the endcell for the search (target)
    std::pair<int, int> endCell_;
    // pair of coordinates of the bottom left corner of the resulting search area
    std::pair<int, int> bottomCorner_;
    // pair of coordinates of the top right corner of the resulting search area
    std::pair<int, int> topCorner_;
    /**
     * @brief Find the search area for the reduced dimensional splitting
     */
    void findSearchArea();

  };

} // namespace Block
