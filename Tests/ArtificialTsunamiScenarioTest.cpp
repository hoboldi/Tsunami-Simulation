#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <../Source/Scenarios/ArtificialTsunamiScenario.h>


TEST_CASE("ArtificialTsunamiScenarioTest") {
  Scenarios::ArtificialTsunamiScenario scenario;
  //Water height should be 100
  REQUIRE(scenario.getWaterHeight(0, 0) == 100);
  //Bathymetry outside the epicenter (x <= 4500 || x >= 5500 || y <= 4500 || y >= 5500) should be -100
  REQUIRE(scenario.getBathymetry(0, 0) == -100);
  //Calculate displacement for displacement (50,50) with epsilon = 0.0001
  REQUIRE_THAT(scenario.calculateDisplacement(50, 50), Catch::Matchers::WithinAbs(-1.529634122, 0.0001));
  //Calculate displacement for displacement (0,0) with epsilon = 0.0001
  REQUIRE_THAT(scenario.calculateDisplacement(0, 0), Catch::Matchers::WithinAbs(0, 0.0001));
  //Calculate Bathymetrie for (5050,5050) with epsilon = 0.0001
  REQUIRE_THAT(scenario.getBathymetry(5050, 5050), Catch::Matchers::WithinAbs(-100.000000000 + -1.529634122, 0.0001));

}
