#pragma once

#include <string>
#include <netcdf>

namespace Readers{
  class NetCDFUnbufferedReader{
    public:
      explicit NetCDFUnbufferedReader(const std::string& filename);

      [[nodiscard]] double readUnbuffered(int x, int y) const;

      size_t getXDim() const { return xDim; }
      size_t getYDim() const { return yDim; }

    private:
      int fileID_;
      size_t xDim, yDim;
      int xVarID_, yVarID_, zVarID_;
};
} // namespace Readers