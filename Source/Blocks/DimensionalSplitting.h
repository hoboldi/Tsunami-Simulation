

#pragma once

#include "Block.hpp"

namespace Blocks {

 class DimensionalSplitting: public Block {

   //! net-updates for the heights of the cells (for accumulation)
   Tools::Float2D<RealType> hNetUpdates_;

   //! net-updates for the x-momentums of the cells (for accumulation)
   Tools::Float2D<RealType> huNetUpdates_;

   //! net-updates for the y-momentums of the cells (for accumulation)
   Tools::Float2D<RealType> hvNetUpdates_;

 public:
   /**
    * Constructor of a Blocks::WaveAccumulationBlock.
    *
    * Allocates the variables for the simulation:
    *   unknowns h,hu,hv,b are defined on grid indices [0,..,nx+1]*[0,..,ny+1] (-> Abstract class Blocks::Block)
    *     -> computational domain is [1,..,nx]*[1,..,ny]
    *     -> plus ghost cell layer
    *
    * Similar, all net-updates are defined as cell-local variables with indices [0,..,nx+1]*[0,..,ny+1],
    * however, only values on [1,..,nx]*[1,..,ny] are used (i.e., ghost layers are not accessed).
    * Net updates are intended to hold the accumulated(!) net updates computed on the edges.
    */
   DimensionalSplitting(int nx, int ny, RealType dx, RealType dy);
   ~DimensionalSplitting() override = default;

   /**
    * Compute net updates for the block.
    * The member variable #maxTimestep will be updated with the
    * maximum allowed time step size
    */
   void computeNumericalFluxes() override;

   /**
    * Updates the unknowns with the already computed net-updates.
    *
    * @param dt time step width used in the update.
    */
   void updateUnknowns(RealType dt) override;
 };

} // namespace Blocks