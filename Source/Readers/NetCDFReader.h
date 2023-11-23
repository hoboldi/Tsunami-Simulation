#pragma once

#include <string>
#include <cassert>
#include <netcdf>
#include "Tools/Float2D.hpp"
#include "Tools/RealType.hpp"
namespace Readers{
  class NetCDFReader{
    public:
      /**
       * @brief A method used for reading a netCDF file and retrieving the necessary values: Timestep, passed time, Bathymetries, Heights, and Momenta to the saved point in time
       * 
       * @param [in] filename: The name of the file which will be read -> The checkpoint file
       * @param [in/out] dx: A pointer which will be filled with the calculated cell-size in x direction
       * @param [in/out] dy: A pointer which will be filled with the calculated cell-size in y direction
       * @param [in/out] nx: A pointer which will be filled with the calculated cell-count in x direction
       * @param [in/out] ny: A pointer which will be filled with the calculated cell-count in y direction
       * 
       * @return [out] bz_data: A pointer to an array of bathymetries, either displacement or bathymetrie after the earthquake (depending on the file)
      */
      static double* readFile(const std::string& filename, RealType& dx, RealType& dy, int& nx, int& ny);

      /**
       * @brief A method used for reading a checkpoint and retrieving the necessary values: Timestep, passed time, Bathymetries, Heights, and Momenta to the saved point in time
       * 
       * @param [in] filename: The name of the file which will be read -> The checkpoint file
       * @param [in/out] timePassed: A pointer which will be filled with the timePassed value read from the checkpoint
       * @param [in/out] bathymetries: A pointer to an array of bathymtries read from the checkpint
       * @param [in/out] heights: A pointer to an array of heights read from the checkpoint
       * @param [in/out] momentaX: A pointer to an array of momenta in X direction read from the checkpoint
       * @param [in/out] momentaY: A pointer to an array of momenta in Y direction read from the checkpoint
       * @param [in/out] dx: A pointer which will be filled with the calculated cell-size in x direction
       * @param [in/out] dy: A pointer which will be filled with the calculated cell-size in y direction
       * @param [in/out] nx: A pointer which will be filled with the calculated cell-count in x direction
       * @param [in/out] ny: A pointer which will be filled with the calculated cell-count in y direction
       * 
       * @return [out] timestep: The timestep of the simulation where the Snapshot was made
       * 
       * Data that has to be in the checkpoint: timestep, timePassed, boundary, dx, dy, nx, ny, b, h, hu, bv
      */
      static int readCheckpoint(const std::string& filename, double* timePassed, double* bathymetries, double* heights, double* hus, double* hvs, int* boundaries, RealType& dx, RealType& dy, int& nx, int& ny);
  };
}