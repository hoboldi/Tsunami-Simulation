#include "NetCDFReader.h"

double* Readers::NetCDFReader::readFile(const std::string& filename, RealType& dx, RealType& dy, int& nx, int& ny) {
  int bncid;
  int retval;

  retval = nc_open(filename.c_str(), NC_NOWRITE, &bncid);
  assert(retval == NC_NOERR);
  // Get the variables
  int    bx_dimid, by_dimid;
  int    bx_varid, by_varid, bz_varid;
  size_t bxlen, bylen;

  // Get dimension ids
  retval = nc_inq_dimid(bncid, "x", &bx_dimid);
  assert(retval == NC_NOERR);
  retval = nc_inq_dimid(bncid, "y", &by_dimid);
  assert(retval == NC_NOERR);

  // Get variable ids
  retval = nc_inq_varid(bncid, "x", &bx_varid);
  assert(retval == NC_NOERR);
  retval = nc_inq_varid(bncid, "y", &by_varid);
  assert(retval == NC_NOERR);
  retval = nc_inq_varid(bncid, "z", &bz_varid);
  assert(retval == NC_NOERR);

  // Get dimension lengths
  retval = nc_inq_dimlen(bncid, bx_dimid, &bxlen);
  assert(retval == NC_NOERR);
  retval = nc_inq_dimlen(bncid, by_dimid, &bylen);
  assert(retval == NC_NOERR);

  double bx_data[bxlen];
  double by_data[bylen];
  auto* bz_data = new double[bxlen * bylen];

  retval = nc_get_var_double(bncid, bx_varid, bx_data);
  assert(retval == NC_NOERR);
  retval = nc_get_var_double(bncid, by_varid, by_data);
  assert(retval == NC_NOERR);
  retval = nc_get_var_double(bncid, bz_varid, bz_data);
  assert(retval == NC_NOERR);
  // get domain of x
  double x0 = bx_data[0];
  double x1 = bx_data[bxlen - 1];
  // calculate dx
  dx = (x1 - x0) / (bxlen - 1);
  // get domain of y
  double y0 = by_data[0];
  double y1 = by_data[bylen - 1];
  // calculate dy
  dy = (y1 - y0) / (bylen - 1);
  // get number of cells in x and y direction scaled by dx and dy
  nx = static_cast<int>(bxlen * dx);
  ny = static_cast<int>(bylen * dy);
  return bz_data;
}
