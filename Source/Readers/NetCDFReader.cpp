#include "NetCDFReader.h"

double* Readers::NetCDFReader::readFile(const std::string& filename, RealType& dx, RealType& dy, int& nx, int& ny) {
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

int Readers::NetCDFReader::readCheckpoint(const std::string& filename, double* timePassed, double* bathymetries, double* heights, double* hus, double* hvs, int* boundaries, RealType& dx, RealType& dy, int& nx, int& ny) {
  int bncid;
  int retval;

  retval = nc_open(filename.c_str(), NC_NOWRITE, &bncid);
  assert(retval == NC_NOERR);
  // Create the variables
  int    timestep_id, timePassed_id, boundary_id;
  int    bx_dimid, by_dimid;
  int    dx_id, dy_id, nx_id, ny_id;
  int    b_varid, h_varid, hu_varid, hv_varid;
  size_t bxlen, bylen;

  //Get the time-related ids
  retval = nc_inq_varid(bncid, "timestep", &timestep_id);
  assert(retval == NC_NOERR);
  retval = nc_inq_varid(bncid, "timePassed", &timePassed_id);
  assert(retval == NC_NOERR);

  //Get boundary data
  retval = nc_inq_varid(bncid, "boundary", &boundary_id);
  assert(retval == NC_NOERR);

  // Get cell-data
  retval = nc_inq_varid(bncid, "dx", &dx_id);
  assert(retval == NC_NOERR);
  retval = nc_inq_varid(bncid, "dy", &dy_id);
  assert(retval == NC_NOERR);
  retval = nc_inq_varid(bncid, "nx", &nx_id);
  assert(retval == NC_NOERR);
  retval = nc_inq_varid(bncid, "ny", &ny_id);
  assert(retval == NC_NOERR);

  int bxlen = nx / dx;
  int bylen = ny / dy;

  // Get dimension ids
  retval = nc_inq_dimid(bncid, "x", &bx_dimid);
  assert(retval == NC_NOERR);
  retval = nc_inq_dimid(bncid, "y", &by_dimid);
  assert(retval == NC_NOERR);
  retval = nc_inq_varid(bncid, "dx", &dx_id);
  assert(retval == NC_NOERR);
  retval = nc_inq_varid(bncid, "dy", &dy_id);
  assert(retval == NC_NOERR);
  retval = nc_inq_varid(bncid, "nx", &nx_id);
  assert(retval == NC_NOERR);
  retval = nc_inq_varid(bncid, "ny", &ny_id);
  assert(retval == NC_NOERR);

  // Get dimension data
  retval = nc_get_var_double(bncid, dx_id, dx);
  assert(retval == NC_NOERR);
  retval = nc_get_var_double(bncid, dy_id, dy);
  assert(retval == NC_NOERR);
  retval = nc_get_var_int(bncid, nx_id, nx);
  assert(retval == NC_NOERR);
  retval = nc_get_var_int(bncid, ny_id, ny);
  assert(retval == NC_NOERR);

  // Get variable ids
  retval = nc_inq_varid(bncid, "b", &b_varid);
  assert(retval == NC_NOERR);
  retval = nc_inq_varid(bncid, "h", &h_varid);
  assert(retval == NC_NOERR);
  retval = nc_inq_varid(bncid, "hu", &hu_varid);
  assert(retval == NC_NOERR);
  retval = nc_inq_varid(bncid, "hv", &hv_varid);
  assert(retval == NC_NOERR);

  // Get dimension lengths
  retval = nc_inq_dimlen(bncid, bx_dimid, &bxlen);
  assert(retval == NC_NOERR);
  retval = nc_inq_dimlen(bncid, by_dimid, &bylen);
  assert(retval == NC_NOERR);

  double bx_data[bxlen];
  double by_data[bylen];
  auto* b_data = new double[bxlen * bylen];
  bathymetries = b_data;
  auto* h_data = new double[bxlen * bylen];
  heights = h_data;
  auto* hu_data = new double[bxlen * bylen];
  hus = hu_data;
  auto* hv_data = new double[bxlen * bylen];
  hvs = hv_data;
  int timestep;
  double timePassed;
  int* boundary_data = new int[4];
  boundaries = boundary_data;

  // Get time-data
  retval = nc_get_var_int(bncid, timestep_id, timestep);
  assert(retval == NC_NOERR);
  retval = nc_get_var_double(bncid, timePassed_id, timePassed);
  assert(retval == NC_NOERR);

  // Get data
  retval = nc_get_var_double(bncid, bx_varid, bx_data);
  assert(retval == NC_NOERR);
  retval = nc_get_var_double(bncid, by_varid, by_data);
  assert(retval == NC_NOERR);
  retval = nc_get_var_double(bncid, b_varid, b_data);
  assert(retval == NC_NOERR);
  retval = nc_get_var_double(bncid, h_varid, h_data);
  assert(retval == NC_NOERR);
  retval = nc_get_var_double(bncid, hu_varid, hu_data);
  assert(retval == NC_NOERR);
  retval = nc_get_var_double(bncid, hv_varid, hv_data);
  assert(retval == NC_NOERR);
  retval = nc_get_var_double(bncid, boundary_id, boundary_data);
  assert(retval == NC_NOERR);

  return timestep;
}
