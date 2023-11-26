#include "NetCDFReader.h"

//#include <format>

#include "Tools/Logger.hpp"

double* Readers::NetCDFReader::rotateAndFlip(size_t bxlen, size_t bylen, const double* bz_data) { // rotate bz_data by 90 degrees cw to match the orientation of the grid
  double* bz_data_rotated = new double[bxlen * bylen];
  for (int i = 0; i < bxlen; i++) {
    for (int j = 0; j < bylen; j++) {
      bz_data_rotated[i * bylen + j] = bz_data[(bylen - 1 - j) * bxlen + i];
    }
  }
  // flip bz_data_rotated on the horizontal axis to match the orientation of the grid
  for (int i = 0; i < bxlen; i++) {
    for (int j = 0; j < bylen / 2; j++) {
      double temp                                = bz_data_rotated[i * bylen + j];
      bz_data_rotated[i * bylen + j]             = bz_data_rotated[i * bylen + bylen - 1 - j];
      bz_data_rotated[i * bylen + bylen - 1 - j] = temp;
    }
  }
  // delete old bz_data
  delete[] bz_data;
  return bz_data_rotated;
}

double* Readers::NetCDFReader::readFile(const std::string& filename, const std::string& varName, RealType& dx, RealType& dy, int& nx, int& ny) {
  int bncid;
  int retval;

  retval = nc_open(filename.c_str(), NC_NOWRITE, &bncid);
  assert(retval == NC_NOERR);
  // Create the variables
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
  retval = nc_inq_varid(bncid, varName.c_str(), &bz_varid);
  assert(retval == NC_NOERR);

  // Get dimension lengths
  retval = nc_inq_dimlen(bncid, bx_dimid, &bxlen);
  assert(retval == NC_NOERR);
  retval = nc_inq_dimlen(bncid, by_dimid, &bylen);
  assert(retval == NC_NOERR);

  double bx_data[bxlen];
  double by_data[bylen];
  auto*  bz_data = new double[bxlen * bylen];

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
  dy                      = (y1 - y0) / (bylen - 1);
  double* bz_data_rotated = rotateAndFlip(bxlen, bylen, bz_data);

  // get number of cells in x and y direction scaled by dx and dy
  nx = static_cast<int>(bxlen * dx);
  ny = static_cast<int>(bylen * dy);
  return bz_data_rotated;
}

double Readers::NetCDFReader::readCheckpoint(
  const std::string& filename, double& timePassed, double*& bathymetries, double*& heights, double*& hus, double*& hvs, int& boundaries, RealType& dx, RealType& dy, int& nx, int& ny
) {
  int bncid;
  int retval;

  retval = nc_open(filename.c_str(), NC_NOWRITE, &bncid);
  assert(retval == NC_NOERR);
  // Create the variables
  int    time_id, boundary_id;
  int    bx_dimid, by_dimid;
  int    dx_id, dy_id, nx_id, ny_id;
  int    b_varid, h_varid, hu_varid, hv_varid, bx_varid, by_varid;
  size_t bxlen, bylen, timeLen;

  // Get the time-related ids
  retval = nc_inq_varid(bncid, "time", &time_id);
  assert(retval == NC_NOERR);


  // Get boundary data
  retval = nc_inq_varid(bncid, "boundary", &boundary_id);
  assert(retval == NC_NOERR);


  // Get variable ids
  retval = nc_inq_varid(bncid, "x", &bx_varid);
  assert(retval == NC_NOERR);
  retval = nc_inq_varid(bncid, "y", &by_varid);
  assert(retval == NC_NOERR);
  retval = nc_inq_varid(bncid, "b", &b_varid);
  assert(retval == NC_NOERR);
  retval = nc_inq_varid(bncid, "h", &h_varid);
  assert(retval == NC_NOERR);
  retval = nc_inq_varid(bncid, "hu", &hu_varid);
  assert(retval == NC_NOERR);
  retval = nc_inq_varid(bncid, "hv", &hv_varid);
  assert(retval == NC_NOERR);

  // Get dimension lengths
  retval = nc_inq_dimlen(bncid, bx_varid, &bxlen);
  assert(retval == NC_NOERR);
  retval = nc_inq_dimlen(bncid, by_varid, &bylen);
  assert(retval == NC_NOERR);
  retval = nc_inq_dimlen(bncid, time_id, &timeLen);
  assert(retval == NC_NOERR);

  double bx_data[bxlen];
  double by_data[bylen];
  double time_data[timeLen];
  auto*  b_data  = new double[bxlen * bylen];
  auto*  h_data  = new double[bxlen * bylen];
  auto*  hu_data = new double[bxlen * bylen];
  auto*  hv_data = new double[bxlen * bylen];




  // Get time-data
  retval = nc_get_var_double(bncid, time_id, time_data);
  assert(retval == NC_NOERR);
  timePassed = time_data[timeLen - 1];
  // print timestep
#ifndef NDEBUG

  //Tools::Logger::logger.printString(std::format("Found {} timesteps in file {}.", timeLen, filename));
  //Tools::Logger::logger.printString(std::format("Last timestep is {}.", timePassed));
#endif

  // Get data
  retval = nc_get_var_double(bncid, bx_varid, bx_data);
  assert(retval == NC_NOERR);
  retval = nc_get_var_double(bncid, by_varid, by_data);
  assert(retval == NC_NOERR);

  dx = (bx_data[bxlen - 1] - bx_data[0]) / (bxlen - 1);
  dy = (by_data[bylen - 1] - by_data[0]) / (bylen - 1);
  nx = static_cast<int>(bxlen);
  ny = static_cast<int>(bylen);

  // Startvector to get the last timestep
  std::size_t start[] = {static_cast<std::size_t>(timeLen - 1), 0, 0};
  std::size_t count[] = {1, static_cast<std::size_t>(bxlen), static_cast<std::size_t>(bylen)};

  retval = nc_get_var_double(bncid, b_varid, b_data);
  assert(retval == NC_NOERR);
  retval = nc_get_vara_double(bncid, h_varid, start, count, h_data);
  assert(retval == NC_NOERR);
  retval = nc_get_vara_double(bncid, hu_varid, start, count, hu_data);
  assert(retval == NC_NOERR);
  retval = nc_get_vara_double(bncid, hv_varid, start, count, hv_data);
  assert(retval == NC_NOERR);
  retval = nc_get_var_int(bncid, boundary_id, &boundaries);
  assert(retval == NC_NOERR);


  bathymetries = rotateAndFlip(bxlen, bylen, b_data);
  heights      = rotateAndFlip(bxlen, bylen, h_data);
  hus          = rotateAndFlip(bxlen, bylen, hu_data);
  hvs          = rotateAndFlip(bxlen, bylen, hv_data);

  // close file
  retval = nc_close(bncid);
  assert(retval == NC_NOERR);

  return time_data[timeLen - 1];
}
