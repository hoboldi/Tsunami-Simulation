#pragma once
#include "Block.hpp"
#include "Solvers/FWaveSolver.h"
namespace Blocks {
  class DimensionalSplitting: public Block {
  private:
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
    DimensionalSplitting(int nx, int ny, RealType dx, RealType dy);
    ~DimensionalSplitting() override = default;

    void computeNumericalFluxes() override;

    void updateUnknowns(RealType dt) override;
  };


} // namespace Blocks