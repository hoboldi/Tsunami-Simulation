#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <iostream>
#include <Tools/Coarse.h>

void printFloat2D(const Tools::Float2D<RealType>& array, int dimX, int dimY) {
  std::cout << "\nPrinting Float2D:" << std::endl;
  for (int y = 0; y < dimY + 2; y++) {
    for (int x = 0; x < dimX + 2; x++) {
      std::cout << array[x][y] << "\t";
    }
    std::cout << "\n";
  }
}

TEST_CASE("Coarse Array Test") {
  SECTION("Simple Test with all -10, coarse by 2x2") {
    Tools::Float2D<RealType> array(12, 12, true);
    for (int i = 0; i < 12; i++) {
      for (int j = 0; j < 12; j++) {
        if (i == 0 || i == 11 || j == 0 || j == 11) {
          array[i][j] = 0;
        } else {
          array[i][j] = -10;
        }
      }
    }
    Tools::Float2D<RealType> coarseArray = Tools::Coarse::coarseArray(array, 2, 10, 10, 5, 0, 5, 0);
    for (int i = 1; i < 6; i++) {
      for (int j = 1; j < 6; j++) {
        REQUIRE(coarseArray[i][j] == -10);
      }
    }
  }

  SECTION("Advanced Test with coarse by 3x3") {
    // all 3x3 blocks sum up to 39, on the boarder that does not fit into a 3x3 block, the sums are 14 except for the bottom right corner which is 15
    // that gives an average of 4.333 for the 3x3 blocks and 4.666 for the boarder cells except for the bottom right corner which is 5
    RealType array[15][15] = {
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 5, 4, 5, 6, 2, 6, 5, 4, 5, 6, 2, 6, 5, 0},
      {0, 4, 3, 4, 5, 1, 5, 4, 3, 4, 5, 1, 5, 4, 0},
      {0, 5, 4, 5, 6, 2, 6, 5, 4, 5, 6, 2, 6, 5, 0},
      {0, 6, 5, 6, 7, 3, 7, 6, 5, 6, 7, 3, 7, 6, 0},
      {0, 2, 1, 2, 3, -1, 3, 2, 1, 2, 3, -1, 3, 2, 0},
      {0, 6, 5, 6, 7, 3, 7, 6, 5, 6, 7, 3, 7, 6, 0},
      {0, 5, 4, 5, 6, 2, 6, 5, 4, 5, 6, 2, 6, 5, 0},
      {0, 4, 3, 4, 5, 1, 5, 4, 3, 4, 5, 1, 5, 4, 0},
      {0, 5, 4, 5, 6, 2, 6, 5, 4, 5, 6, 2, 6, 5, 0},
      {0, 6, 5, 6, 7, 3, 7, 6, 5, 6, 7, 3, 7, 6, 0},
      {0, 2, 1, 2, 3, -1, 3, 2, 1, 2, 3, -1, 3, 2, 0},
      {0, 6, 5, 6, 7, 3, 7, 6, 5, 6, 7, 3, 7, 6, 0},
      {0, 5, 4, 5, 6, 2, 6, 5, 4, 5, 6, 2, 6, 5, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};


    Tools::Float2D<RealType> arr(15, 15, *array);

    Tools::Float2D<RealType> coarseArray    = Tools::Coarse::coarseArray(arr, 3, 13, 13, 4, 1, 4, 1);
    RealType                 expected[7][7] = {
      {0, 0, 0, 0, 0, 0, 0},
      {0, 4.33333, 4.33333, 4.33333, 4.33333, 4.6666, 0},
      {0, 4.33333, 4.33333, 4.33333, 4.33333, 4.6666, 0},
      {0, 4.33333, 4.33333, 4.33333, 4.33333, 4.6666, 0},
      {0, 4.33333, 4.33333, 4.33333, 4.33333, 4.6666, 0},
      {0, 4.6666, 4.6666, 4.6666, 4.6666, 5, 0},
      {0, 0, 0, 0, 0, 0, 0}};
    for (int i = 1; i < 6; i++) {
      for (int j = 1; j < 6; j++) {
        REQUIRE_THAT(coarseArray[i][j], Catch::Matchers::WithinRel(expected[i][j], 1e-3));
      }
    }
  }
}