#pragma once
#include "DimensionalSplitting.h"
#include "Solvers/FWaveSolver.h"
#if defined(ENABLE_GUI)
#include "Gui/Gui.h"
#endif

namespace Blocks {
  class ReducedDimSplittingBlock : public DimensionalSplitting {
  public:
    ReducedDimSplittingBlock(int nx, int ny, RealType dx, RealType dy, std::pair<int, int> startCell, std::pair<int, int> endCell);
    ~ReducedDimSplittingBlock() override = default;
#if defined(ENABLE_GUI)
    /**
     * @brief Find the search area for the reduced dimensional splitting with GUI visualization
     */
    void findSearchArea(Gui::Gui& gui);
#endif
    /**
     * @brief Find the search area for the reduced dimensional splitting
     */
    void findSearchArea();

    void setStartCell(std::pair<int, int> startCell);
    void setEndCell(std::pair<int, int> endCell);

    void computeNumericalFluxes() override;

    void updateUnknowns(RealType dt) override;


  private:
    // pair of coordinates of the startcell for the search (epicenter)
    std::pair<int, int> startCell_;
    // pair of coordinates of the endcell for the search (target)
    std::pair<int, int> endCell_;
    // pair of coordinates of the bottom left corner of the resulting search area
    std::pair<int, int> bottomCorner_;
    // pair of coordinates of the top right corner of the resulting search area
    std::pair<int, int> topCorner_;


  };

} // namespace Block
