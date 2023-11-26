#pragma once

#include <string>
#include <cassert>
#include <netcdf>
#include "Tools/Float2D.hpp"
#include "Tools/RealType.hpp"
namespace Readers{
  class NetCDFReader{
    public:

      static double* readFile(const std::string& filename, RealType& dx, RealType& dy, int& nx, int& ny);
  };
}