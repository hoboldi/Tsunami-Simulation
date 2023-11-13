#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <iostream>

#include "../SWE1D/Blocks/WavePropagationBlock1D.h"
#include "../SWE1D/Scenarios/BathymetryScenario.h"
#include "Blocks/DimensionalSplitting.h"

/*
// adapter height function for the 1d scenario into 2d
RealType h_func(RealType x, RealType t) {
  //since all cell-sizes are ints the x value is always an int
  x /= scenario.getCellSize();
  x += 0.5;
  return scenario.getHeight(static_cast<unsigned int>(x));
}
// adapter momentum function for the 1d scenario into 2d
RealType hu_func(RealType x, RealType t) {
  //since all cell-sizes are ints the x value is always an int
  x /= scenario.getCellSize();
  x += 0.5;
  return scenario.getMomentum(static_cast<unsigned int>(x));
}
// adapter bathymetry function for the 1d scenario into 2d
RealType b_func(RealType x, RealType t) {
  //since all cell-sizes are ints the x value is always an int
  x /= scenario.getCellSize();
  x += 0.5;
  return scenario.getBathymetry(static_cast<unsigned int>(x));
}
// function to zero y-sweep
RealType zero_func(RealType x, RealType t) {
  return 0;
}*/



TEST_CASE("Comparison of the 2d DimensionalSplitting Block and the 1d WavePropagation Block") {
  unsigned int size = 100;
  Scenarios::BathymetryScenario scenario(size);
  //create the 2d DimensionalSplitting Block
  Blocks::DimensionalSplitting dimensionalSplittingBlock((int)size, 1, scenario.getCellSize(), scenario.getCellSize());
  //create the 1d WavePropagation Block
  // Allocate memory
  // Water height
  RealType* h = new RealType[size + 2];
  // Momentum
  RealType* hu = new RealType[size + 2];
  //Bathymetry
  RealType* b = new RealType[size + 2];

  // Initialize water height, momentum, bathymetry
  for (unsigned int i = 0; i < size + 2; i++) {
    h[i] = scenario.getHeight(i);
  }

  for (unsigned int i = 0; i < size + 2; i++) {
    hu[i] = scenario.getMomentum(i);
  }

  for (unsigned int i = 0; i < size + 2; i++) {
    b[i] = scenario.getBathymetry(i);
  }

  Blocks::WavePropagationBlock1D wavePropagationBlock(h, hu, b, size, scenario.getCellSize());
  // init the 2d DimensionalSplitting Block

  Tools::Float2D<RealType> h2d(size + 2, 3);
  Tools::Float2D<RealType> hu2d(size + 2, 3);
  Tools::Float2D<RealType> hv2d(size + 2, 3);
  Tools::Float2D<RealType> b2d(size + 2, 3);
  for (int i = 0; i < size + 2; i++) {
    for (int j = 0; j < 3; ++j) {
      if (j == 1 && i < size + 1 && i > 0){
        h2d[i][1] = h[i];
        hu2d[i][1] = hu[i];
        hv2d[i][1] = 0;
        b2d[i][1] = b[i];
      } else{
        h2d[i][j] = 0;
        hu2d[i][j] = 0;
        hv2d[i][j] = 0;
        b2d[i][j] = 0;
      }
    }

  }
  dimensionalSplittingBlock.setH(h2d);
  dimensionalSplittingBlock.setHv(hv2d);
  dimensionalSplittingBlock.setHu(hu2d);
  dimensionalSplittingBlock.setB(b2d);



  dimensionalSplittingBlock.setGhostLayer();

  // run one timestep
  RealType max1d = wavePropagationBlock.computeNumericalFluxes();
  dimensionalSplittingBlock.computeNumericalFluxes();
  RealType max2d = dimensionalSplittingBlock.getMaxTimeStep();
  // compare the results
  REQUIRE_THAT(max1d, Catch::Matchers::WithinRel(max2d));
  // update the unknowns
  wavePropagationBlock.updateUnknowns(max1d);
  dimensionalSplittingBlock.updateUnknowns(max2d);



  RealType total = 0;
  for (unsigned int i = 1; i < size +1; i++) {
    std::printf("I: %d:%f %f\n",i, h[i], h2d[i][1]);
    REQUIRE_THAT(h[i], Catch::Matchers::WithinAbs(h2d[i][1], 1.5));
  }
  delete[] h;
  delete[] hu;
  delete[] b;

  delete[] h2d.getData();
  delete[] hu2d.getData();
  delete[] hv2d.getData();
  delete[] b2d.getData();
}