#pragma once

#include "Tools/RealType.hpp"


namespace Scenarios {

  class BathymetryScenario {
    /** Number of cells */
    const unsigned int size_;

  public:
    BathymetryScenario(unsigned int size);
    ~BathymetryScenario() = default;

    /**
     * @return Cell size of one cell (= domain size/number of cells)
     */
    RealType getCellSize() const;

    /**
     * @return Initial water height at pos
     */
    unsigned int getHeight(unsigned int pos) const;

    int getMomentum(unsigned int pos) const;


    /**
     * @return Initial bathymetry at pos
     */
    int getBathymetry(unsigned int pos) const;
};


} // namespace Scenarios
