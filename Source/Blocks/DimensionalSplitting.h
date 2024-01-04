#pragma once
#include "Block.hpp"
#include "Solvers/FWaveSolver.h"
namespace Blocks {
  class DimensionalSplitting: public Block {
  protected:
    //! net-updates for the heights of the cells on the x-stride.
    Tools::Float2D<RealType> hNetUpdatesXLeft_;
    Tools::Float2D<RealType> hNetUpdatesXRight_;

    //! net-updates for the x-momentums of the cells on the x-stride.
    Tools::Float2D<RealType> huNetUpdatesXLeft_;
    Tools::Float2D<RealType> huNetUpdatesXRight_;

    //! net-updates for the heights of the cells on the y-stride.
    Tools::Float2D<RealType> hNetUpdatesYLeft_;
    Tools::Float2D<RealType> hNetUpdatesYRight_;

    //! net-updates for the y-momentums of the cells on the y-stride.
    Tools::Float2D<RealType> hvNetUpdatesYLeft_;
    Tools::Float2D<RealType> hvNetUpdatesYRight_;

    Solvers::FWaveSolver fWaveSolver_;

  public:
    /**
     * @brief Construct a new Dimensional Splitting object
     * @param nx cell count in x-direction
     * @param ny cell count in y-direction
     * @param dx cell size in x-direction
     * @param dy cell size in y-direction
     */
    DimensionalSplitting(int nx, int ny, RealType dx, RealType dy);
    ~DimensionalSplitting() override = default;

    /**
     * @brief Compute the net-updates for the x- and y-stride.
     */
    void computeNumericalFluxes() override;
    /**
     * @brief Update the unknowns with the net-updates.
     * @param dt maximum time step size
     */
    void updateUnknowns(RealType dt) override;
    // needed for the tests
    void setHv(const Tools::Float2D<RealType>& hv);
    void setHu(const Tools::Float2D<RealType>& hu);
    void setB(const Tools::Float2D<RealType>& b);
    void setH(const Tools::Float2D<RealType>& h);
  };


} // namespace Blocks