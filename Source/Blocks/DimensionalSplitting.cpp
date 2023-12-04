#include "DimensionalSplitting.h"
#if defined(ENABLE_OPENMP)
  #include <omp.h>
#endif

Blocks::DimensionalSplitting::DimensionalSplitting(int nx, int ny, RealType dx, RealType dy):
  Block(nx, ny, dx, dy),
  hNetUpdatesXLeft_(nx + 1, ny),
  hNetUpdatesXRight_(nx + 1, ny),
  huNetUpdatesXLeft_(nx + 1, ny),
  huNetUpdatesXRight_(nx + 1, ny),
  hNetUpdatesYLeft_(nx, ny + 1),
  hNetUpdatesYRight_(nx, ny + 1),
  hvNetUpdatesYLeft_(nx, ny + 1),
  hvNetUpdatesYRight_(nx, ny + 1) {}


void Blocks::DimensionalSplitting::computeNumericalFluxes() {
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
#pragma omp parallel for reduction(max : maxWaveSpeedX)
#endif
    for (int i = 0; i <= nx_; ++i) {
      for (int j = 1; j <= ny_; ++j) {
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
#pragma omp parallel for reduction(max : maxWaveSpeedY)
#endif
    for (int i = 1; i < nx_; ++i) {
      for (int j = 0; j <= ny_; ++j) {
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

    void Blocks::DimensionalSplitting::updateUnknowns(RealType dt) {

#if defined(ENABLE_OPENMP)
#pragma omp parallel for
#endif
      for (int i = 1; i <= nx_; ++i) {
        for (int j = 1; j < ny_; ++j) {
          h_[i][j] -= dt / dx_ * (hNetUpdatesXRight_[i - 1][j] + hNetUpdatesXLeft_[i][j]);
          hu_[i][j] -= dt / dx_ * (huNetUpdatesXRight_[i - 1][j] + huNetUpdatesXLeft_[i][j]);
        }
      }

#if defined(ENABLE_OPENMP)
#pragma omp parallel for
#endif
      for (int i = 1; i < nx_; ++i) {
        for (int j = 1; j <= ny_; ++j) {
          h_[i][j] -= dt / dy_ * (hNetUpdatesYRight_[i][j - 1] + hNetUpdatesYLeft_[i][j]);
          hv_[i][j] -= dt / dy_ * (hvNetUpdatesYRight_[i][j - 1] + hvNetUpdatesYLeft_[i][j]);
        }
      }
    }
    void Blocks::DimensionalSplitting::setHu(const Tools::Float2D<RealType>& hu) { hu_ = hu; }
    void Blocks::DimensionalSplitting::setHv(const Tools::Float2D<RealType>& hv) { hv_ = hv; }
    void Blocks::DimensionalSplitting::setB(const Tools::Float2D<RealType>& b) { b_ = b; }
    void Blocks::DimensionalSplitting::setH(const Tools::Float2D<RealType>& h) { h_ = h; }
