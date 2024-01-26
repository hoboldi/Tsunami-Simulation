//
// Created by simon on 1/26/24.
//

#include "NetCDFUnbufferedReader.h"

#include <cassert>
Readers::NetCDFUnbufferedReader::NetCDFUnbufferedReader(const std::string& filename) {
  int retval = nc_open(filename.c_str(), NC_NOWRITE, &fileID_);
  assert(retval == NC_NOERR);

  if(retval != NC_NOERR){
    std::cout << "Error opening file " << filename << nc_strerror(retval) << std::endl;
    exit(EXIT_FAILURE);
  }

  // Create the variables
  int    bx_dimid, by_dimid;

  // Get dimension ids
  retval = nc_inq_dimid(fileID_, "lat", &bx_dimid);
  assert(retval == NC_NOERR);
  retval = nc_inq_dimid(fileID_, "lon", &by_dimid);
  assert(retval == NC_NOERR);

  // Get dimension lengths
  retval = nc_inq_dimlen(fileID_, bx_dimid, &xDim);
  assert(retval == NC_NOERR);
  retval = nc_inq_dimlen(fileID_, by_dimid, &yDim);
  assert(retval == NC_NOERR);
  std::cout << "xDim: " << xDim << " yDim: " << yDim << std::endl;

  // Get variable ids
  retval = nc_inq_varid(fileID_, "lat", &xVarID_);
  assert(retval == NC_NOERR);
  retval = nc_inq_varid(fileID_, "lon", &yVarID_);
  assert(retval == NC_NOERR);
  retval = nc_inq_varid(fileID_, "elevation", &zVarID_);


}
double Readers::NetCDFUnbufferedReader::readUnbuffered(const int x, const int y) const {
  double data;
  size_t indexptr[2] = {static_cast<size_t>(x), static_cast<size_t>(y)};
  int retval = nc_get_var1_double(fileID_, zVarID_, indexptr, &data);
  if(retval != NC_NOERR){
    std::cout << "Error reading data" << nc_strerror(retval) <<std::endl;
    std::cout << "x: " << x << " y: " << y << std::endl;
    exit(EXIT_FAILURE);
  }
  assert(retval == NC_NOERR);
  return data;
}
