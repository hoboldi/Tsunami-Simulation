#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <netcdf.h>
#include <netcdf>
#include <string>
#include <utility>
#include <vector>
#include "RealType.hpp"

namespace Tools {

  struct interval {
    RealType xleft;
    RealType xright;
    RealType yleft;
    RealType yright;
    RealType h;
    RealType b;
  };
}