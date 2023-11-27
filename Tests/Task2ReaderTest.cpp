#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <iostream>

#include "../Source/Scenarios/TsunamiScenario.h"


TEST_CASE("Comparison of the intervals") {
  Scenarios::TsunamiScenario scenario;
  scenario.readScenario("netCDFReaderTestBathymetry.nc", "netCDFReaderTestDisplacement.nc");

  std::vector<std::vector<Scenarios::interval>> intervalsToBeTested = Scenarios::getInterval();

  std::vector<std::vector<Scenarios::interval>> intervalsExpected(5);
  for (std::size_t i = 0; i < 5; i++) {
    intervalsExpected[i].resize(5);
  }
  intervalsExpected[0][0] = Scenarios::interval{-1.7976931348623157e+308, 0.5, -1.7976931348623157e+308, 0.5, 20, -20};
  intervalsExpected[0][1] = Scenarios::interval{-1.7976931348623157e+308, 0.5, 0.5, 1.5, 20, -30};
  intervalsExpected[0][2] = Scenarios::interval{-1.7976931348623157e+308, 0.5, 1.5, 2.5, 20, -40};
  intervalsExpected[0][3] = Scenarios::interval{-1.7976931348623157e+308, 0.5, 2.5, 3.5, 20, -30};
  intervalsExpected[0][4] = Scenarios::interval{-1.7976931348623157e+308, 0.5, 3.5, 1.7976931348623157e+308, 20, -20};

  intervalsExpected[1][0] = Scenarios::interval{0.5, 1.5, -1.7976931348623157e+308, 0.5, 20, -20};
  intervalsExpected[1][1] = Scenarios::interval{0.5, 1.5, 0.5, 1.5, 30, -40};
  intervalsExpected[1][2] = Scenarios::interval{0.5, 1.5, 1.5, 2.5, 30, -50};
  intervalsExpected[1][3] = Scenarios::interval{0.5, 1.5, 2.5, 3.5, 30, -40};
  intervalsExpected[1][4] = Scenarios::interval{0.5, 1.5, 3.5, 1.7976931348623157e+308, 20, -20};

  intervalsExpected[2][0] = Scenarios::interval{1.5, 2.5, -1.7976931348623157e+308, 0.5, 20, -20};
  intervalsExpected[2][1] = Scenarios::interval{1.5, 2.5, 0.5, 1.5, 30, -40};
  intervalsExpected[2][2] = Scenarios::interval{1.5, 2.5, 1.5, 2.5, 40, -60};
  intervalsExpected[2][3] = Scenarios::interval{1.5, 2.5, 2.5, 3.5, 30, -40};
  intervalsExpected[2][4] = Scenarios::interval{1.5, 2.5, 3.5, 1.7976931348623157e+308, 20, -20};

  intervalsExpected[3][0] = Scenarios::interval{2.5, 3.5, -1.7976931348623157e+308, 0.5, 20, -20};
  intervalsExpected[3][1] = Scenarios::interval{2.5, 3.5, 0.5, 1.5, 30, -40};
  intervalsExpected[3][2] = Scenarios::interval{2.5, 3.5, 1.5, 2.5, 30, -50};
  intervalsExpected[3][3] = Scenarios::interval{2.5, 3.5, 2.5, 3.5, 30, -40};
  intervalsExpected[3][4] = Scenarios::interval{2.5, 3.5, 3.5, 1.7976931348623157e+308, 20, -20};

  intervalsExpected[4][0] = Scenarios::interval{3.5, 1.7976931348623157e+308, -1.7976931348623157e+308, 0.5, 20, -20};
  intervalsExpected[4][1] = Scenarios::interval{3.5, 1.7976931348623157e+308, 0.5, 1.5, 20, -30};
  intervalsExpected[4][2] = Scenarios::interval{3.5, 1.7976931348623157e+308, 1.5, 2.5, 20, -40};
  intervalsExpected[4][3] = Scenarios::interval{3.5, 1.7976931348623157e+308, 2.5, 3.5, 20, -30};
  intervalsExpected[4][4] = Scenarios::interval{3.5, 1.7976931348623157e+308, 3.5, 1.7976931348623157e+308, 20, -20};

  for (size_t i = 0; i < 5; i++) {
    for (size_t j = 0; j < 5; j++) {
      REQUIRE_THAT(intervalsToBeTested[i][j].xleft, Catch::Matchers::WithinAbs(intervalsExpected[i][j].xleft, 0.000001));
      REQUIRE_THAT(intervalsToBeTested[i][j].xright, Catch::Matchers::WithinAbs(intervalsExpected[i][j].xright, 0.000001));
      REQUIRE_THAT(intervalsToBeTested[i][j].yleft, Catch::Matchers::WithinAbs(intervalsExpected[i][j].yleft, 0.000001));
      REQUIRE_THAT(intervalsToBeTested[i][j].yright, Catch::Matchers::WithinAbs(intervalsExpected[i][j].yright, 0.000001));
      REQUIRE_THAT(intervalsToBeTested[i][j].h, Catch::Matchers::WithinAbs(intervalsExpected[i][j].h, 0.000001));
      REQUIRE_THAT(intervalsToBeTested[i][j].b, Catch::Matchers::WithinAbs(intervalsExpected[i][j].b, 0.000001));
    }
  }
}
