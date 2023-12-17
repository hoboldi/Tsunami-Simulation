
// #include <format>
#include <string>

#include "Blocks/DimensionalSplitting.h"
#include "BoundaryEdge.hpp"
#include "Readers/NetCDFReader.h"
#include "Scenarios/CheckpointScenario.h"
#include "Scenarios/RadialDamBreakScenario.hpp"
#include "Scenarios/TsunamiScenario.h"
#include "Scenarios/ArtificialTsunamiScenario.h"
#include "Tools/Args.hpp"
#include "Tools/Logger.hpp"
#include "Tools/ProgressBar.hpp"
#include "Writers/NetCDFWriter.hpp"
#include "Writers/Writer.hpp"
#include <iostream>
#ifdef ENABLE_OPENMP
#include <omp.h>
#endif

void printFloat2D(Tools::Float2D<RealType> array, int dimX, int dimY)
{
  std::cout << "Printing Array:" << std::endl;
  for (int x = 0; x < dimX; x++)
  {
    for (int y = 0; y < dimY; y++)
    {
      std::cout << array[x][y] << "  ";
    }
    std::cout << std::endl;
  }
}

Tools::Float2D<RealType> coarseArray(const Tools::Float2D<RealType>& array, int coarse, int nx, int ny, int groupsX, int restX, int groupsY, int restY)
{
  int addX = 0;
  if (restX != 0)
  {
    addX++;
  }
  int addY = 0;
  if (restY != 0)
  {
    addY++;
  }
  std::cout << "Should have groupX, restX, groupY, restY, addX, addY " << groupsX << ";" << restX << ";" << groupsY << ";" << restY << ";" << addX << ";" << addY << std::endl;
  RealType averagedValue = 0;
  //Tools::Float2D<RealType> tempStorageX(groupsX + addX, ny, true);
  RealType* tempArrayX = new RealType[(groupsX + addX) * ny];
  //std::cout << "Created x Storage" << std::endl;
  //Average the values in x Direction
  for (int y = 0; y < ny; y++)
  {
    //std::cout << "Average x for y " << y << std::endl; 
    averagedValue = 0;
    for (int x = 0; x < groupsX; x++)
    {
      averagedValue = 0;
      for (int i = 0; i < coarse; i++)
      {
        averagedValue += array[x*coarse + i][y];
      }
      averagedValue = averagedValue / coarse;
      //tempStorageX[x][y] = averagedValue;
      //std::cout << "Trying to write to cell " << y*(groupsX + addX) + x << " for array of size " << (groupsX + addX) * ny << std::endl;
      tempArrayX[(y*(groupsX + addX)) + x] = averagedValue;
    }
    averagedValue = 0;
    //Collect the remaining restX cells into one
    //std::cout << "Main X calculation done" << std::endl;
    if (addX != 0)
    {
      //std::cout << "Collecting rest for x..." << std::endl;
      averagedValue = 0;
      for (int i = 0; i < restX; i++)
      {
        //std::cout << "\ti = " << i << std::endl;
        averagedValue += array[groupsX*coarse + i][y];
      }
      averagedValue = averagedValue / coarse;
      //tempStorageX[groupsX][y] = averagedValue;
      tempArrayX[groupsY*(groupsX + addX) + groupsX] = averagedValue;
    }
  }
  //std::cout << "Going for the y averaging now" << std::endl;
  //Average the values in y direction
  //Tools::Float2D<RealType> tempStorageY(groupsX + addX, groupsY + addY, true);
  RealType* tempArrayY = new RealType[(groupsX + addX) * (groupsY + addY)];
  //std::cout << "TempStorage Y allocated" << std::endl;
  for (int x = 0; x < groupsX + addX; x++)
  {
    //std::cout << "Average for x = " << x << std::endl;
    averagedValue = 0;
    for (int y = 0; y < groupsY; y++)
    {
      averagedValue = 0;
      for (int i = 0; i < coarse; i++)
      {
        //averagedValue += tempStorageX[x][y*coarse + i];
        averagedValue += tempArrayX[(y*coarse) + (i*(groupsX + addX)) + x];
      }
      averagedValue = averagedValue / coarse;
      //tempStorageY[x][y] = averagedValue;
      tempArrayY[(y*(groupsX + addX)) + x] = averagedValue;
    }
    //std::cout << "Main Y calculation done" << std::endl;
    // Collect the remaining restY rows below
    if (addY != 0)
    {
      //std::cout << "Collecting rest for y..." << std::endl;
      averagedValue = 0;
      for (int i = 0; i < restY; i++)
      {
        //std::cout << "\ti = " << i << std::endl;
        //averagedValue += tempStorageX[x][groupsY*coarse + i];
        averagedValue += tempArrayX[x + (groupsY*coarse) + (i*(groupsX+addX))];
      }
      averagedValue = averagedValue / coarse;
      //tempStorageY[x][groupsY] = averagedValue;
      tempArrayX[(groupsY + addY)*(groupsY) + x] = averagedValue;
    }
  }
  std::cout << "Done with the calculation" << std::endl;
  Tools::Float2D<RealType> averagedArray(groupsX + addX, groupsY + addY, tempArrayY);
  printFloat2D(averagedArray, groupsX + addX, groupsY + addY);
  std::cout << "All done\n" << std::endl;
  return averagedArray;
}

int main(int argc, char** argv) {
  Tools::Args args;
  args.addOption("grid-size-x", 'x', "Number of cells in x direction");
  args.addOption("grid-size-y", 'y', "Number of cells in y direction");
  args.addOption("output-basepath", 'o', "Output base file name");
  args.addOption("number-of-checkpoints", 'n', "Number of checkpoints to write output files");
  args.addOption("simulation-time", 't', "Simulation time in seconds");
  args.addOption(
    "boundary-conditions",
    'b',
    "Set Boundary Conditions represented by an 4 digit Integer of 1s and 2s. (1: Outflow, 2: Wall).\n First Digit: Left Boundary\n Second Digit: Right Boundary\n Third Digit: Bottom Boundary\n Fourth Digit: Top Boundary"
  );
  args.addOption("checkpoint-file", 'c', "Checkpoint file to read initial values from");
  args.addOption("coarse", 'a', "Parameter for the coarse output, averaging the next <param> cells");

  Tools::Args::Result ret = args.parse(argc, argv);
  if (ret == Tools::Args::Result::Help) {
    return 0;
  }
  if (ret == Tools::Args::Result::Error) {
    return 1;
  }

  // Create the scenario


  int         numberOfGridCellsX = args.getArgument<int>("grid-size-x", 10);
  int         numberOfGridCellsY = args.getArgument<int>("grid-size-y", 10);
  std::string baseName           = args.getArgument<std::string>("output-basepath", "SWE");
  int numberOfCheckPoints = args.getArgument<int>("number-of-checkpoints", 20); //! Number of checkpoints for visualization (at each checkpoint in time, an output file is written).
  double      endSimulationTime  = args.getArgument<double>("simulation-time", 10);
  int         boundaryConditions = args.getArgument<int>("boundary-conditions", 1111); // Default is 1111: Outflow for all Edges
  std::string checkpointFile     = args.getArgument<std::string>("checkpoint-file", "");
  int         coarse             = args.getArgument<int>("coarse", 0); // Default is 0 if no coarse should be used

  std::vector<RealType> coarseHeights;
  std::vector<RealType> coarseHus;
  std::vector<RealType> coarseHvs;

  Tools::Logger::logger.printWelcomeMessage();

  // Print information about the grid
  Tools::Logger::logger.printNumberOfCells(numberOfGridCellsX, numberOfGridCellsY);
  Scenarios::Scenario* scenario;

  if (checkpointFile.empty()) {
    //auto tsunamiScenario = new Scenarios::TsunamiScenario();
    // tsunamiScenario->readScenario("chile_gebco_usgs_2000m_bath.nc", "chile_gebco_usgs_2000m_displ.nc");
    //tsunamiScenario->readScenario("tohoku_gebco_ucsb3_2000m_hawaii_bath.nc", "tohoku_gebco_ucsb3_2000m_hawaii_displ.nc");
    //scenario = tsunamiScenario;
    scenario = new Scenarios::ArtificialTsunamiScenario();
  } else {
    scenario = new Scenarios::CheckpointScenario(checkpointFile);
  }


  if (checkpointFile.empty()) {
    // Ihhgitt!!
    if (boundaryConditions == 1111 || boundaryConditions == 1112 || boundaryConditions == 1121 || boundaryConditions == 1122 || boundaryConditions == 1211 || boundaryConditions == 1212 || boundaryConditions == 1221 || boundaryConditions == 1222 || boundaryConditions == 2111 || boundaryConditions == 2112 || boundaryConditions == 2121 || boundaryConditions == 2122 || boundaryConditions == 2211 || boundaryConditions == 2212 || boundaryConditions == 2221 || boundaryConditions == 2222) {
      scenario->setBoundaryType(boundaryConditions);
    } else {
      std::cout << "Boundary conditions invalid!" << std::endl;
      return 1;
    }
  }


  if (endSimulationTime >= 0) {
    scenario->setEndSimulationTime(endSimulationTime);
  } else {
    std::cout << "Simulation time must be positive" << std::endl;
    return 1;
  }


  // Compute the size of a single cell
  RealType cellSizeX = (scenario->getBoundaryPos(BoundaryEdge::Right) - scenario->getBoundaryPos(BoundaryEdge::Left)) / numberOfGridCellsX;
  RealType cellSizeY = (scenario->getBoundaryPos(BoundaryEdge::Top) - scenario->getBoundaryPos(BoundaryEdge::Bottom)) / numberOfGridCellsY;

  auto waveBlock = new Blocks::DimensionalSplitting(numberOfGridCellsX, numberOfGridCellsY, cellSizeX, cellSizeY);
  waveBlock->initialiseScenario(0, 0, *scenario);

  double* checkPoints = new double[numberOfCheckPoints + 1];

  for (int cp = 0; cp <= numberOfCheckPoints; cp++) {
    checkPoints[cp] = cp * (endSimulationTime / numberOfCheckPoints);
  }

  Writers::BoundarySize boundarySize = {{1, 1, 1, 1}};
  int groupsX = 0;
  int groupsY = 0;
  int restX = 0;
  int restY = 0;
  int addX = 0;
  int addY = 0;
  if (coarse > 0)
  {
    groupsX = waveBlock->getNx() / coarse;
    restX = waveBlock->getNx() - (groupsX * coarse);
    groupsY = waveBlock->getNy() / coarse;
    restY = waveBlock->getNy() - (groupsY * coarse);
    addX = (restX != 0) ? 1 : 0;
    addY = (restY != 0) ? 1 : 0;
  }
  Writers::NetCDFWriter writer = checkpointFile.empty() ? Writers::NetCDFWriter(
        baseName,
        (coarse <= 0) ? waveBlock->getBathymetry() : coarseArray(waveBlock->getBathymetry(), coarse, waveBlock->getNx(), waveBlock->getNy(), groupsX, restX, groupsY, restY),
        boundarySize,
        boundaryConditions,
        (coarse <= 0) ? numberOfGridCellsX : groupsX + addX,
        (coarse <= 0) ? numberOfGridCellsY : groupsY + addY,
        cellSizeX,
        cellSizeY,
        scenario->getBoundaryPos(BoundaryEdge::Left),
        scenario->getBoundaryPos(BoundaryEdge::Bottom),
        1
      )
      : Writers::NetCDFWriter(checkpointFile, (coarse <= 0) ? numberOfGridCellsX : groupsX + addX, (coarse <= 0) ? numberOfGridCellsY : groupsY + addY, boundarySize, 1);
  Tools::ProgressBar progressBar(endSimulationTime);
  progressBar.update(0.0);
  if (checkpointFile.empty()) {
    
    // Coarse output here
    if (coarse > 0)
    {
      // Save the Arrays seperately to prevent memory issues
      // average the values in the arrays
      writer.writeTimeStep
      (coarseArray(waveBlock->getWaterHeight(), coarse, waveBlock->getNx(), waveBlock->getNy(), groupsX, restX, groupsY, restY),
       coarseArray(waveBlock->getDischargeHu(), coarse, waveBlock->getNx(), waveBlock->getNy(), groupsX, restX, groupsY, restY),
       coarseArray(waveBlock->getDischargeHv(), coarse, waveBlock->getNx(), waveBlock->getNy(), groupsX, restX, groupsY, restY),
       0.0);
    }
    else
    {
      writer.writeTimeStep(waveBlock->getWaterHeight(), waveBlock->getDischargeHu(), waveBlock->getDischargeHv(), 0.0);
    }
  std::cout << "Wrote first steps" << std::endl;
  /*
   if (coarse > 0)
   {
    int groupsX = waveBlock->getNx() / coarse;
    int restX = waveBlock->getNx() - (groupsX * coarse);
    int groupsY = waveBlock->getNy() / coarse;
    int restY = waveBlock->getNy() - (groupsY * coarse);
    coarseArray(waveBlock->getWaterHeight(), coarse, waveBlock->getNx(), waveBlock->getNy(), groupsX, restX, groupsY, restY);
    coarseArray(waveBlock->getDischargeHu(), coarse, waveBlock->getNx(), waveBlock->getNy(), groupsX, restX, groupsY, restY);
    coarseArray(waveBlock->getDischargeHv(), coarse, waveBlock->getNx(), waveBlock->getNy(), groupsX, restX, groupsY, restY);
   }
   writer.writeTimeStep(waveBlock->getWaterHeight(), waveBlock->getDischargeHu(), waveBlock->getDischargeHv(), 0.0);
  }
  */
  double simulationTime = scenario->getStartTime();
  progressBar.update(simulationTime);
  // skip until correct checkpoint
  int cp = 1;
  while (simulationTime > checkPoints[cp] && cp <= numberOfCheckPoints) {
    cp++;
  }


  /*
    if (!checkpointFile.empty() && simulationTime != 0) {
      Tools::Logger::logger.printString(
        std::format("Checkpoint file {} loaded, ignoring previously defined output file: {} and appending to {}.", checkpointFile, baseName, checkpointFile)
      );
    }
    */

  Tools::Logger::logger.printStartMessage();
  Tools::Logger::logger.initWallClockTime(time(NULL));

  unsigned int iterations = 0;
  // Loop over checkpoints
  for (; cp <= numberOfCheckPoints; cp++) {
    // Do time steps until next checkpoint is reached
    while (simulationTime < checkPoints[cp]) {
      // Reset CPU-Communication clock
      Tools::Logger::logger.resetClockToCurrentTime("CPU-Communication");

      // Reset the cpu clock
      Tools::Logger::logger.resetClockToCurrentTime("CPU");
      // Set values in ghost cells
      waveBlock->setGhostLayer();
      // Compute numerical flux on each edge
      waveBlock->computeNumericalFluxes();
      RealType maxTimeStepWidth = waveBlock->getMaxTimeStep();
      std::cout << "GetMaxTimestep called Value: " << maxTimeStepWidth << std::endl;
      // Update the cell values
      waveBlock->updateUnknowns(maxTimeStepWidth);
      // Update the cpu time in the logger
      Tools::Logger::logger.updateTime("CPU");
      Tools::Logger::logger.updateTime("CPU-Communication");

      // Print the current simulation time
      progressBar.clear();
      Tools::Logger::logger.printSimulationTime(
        simulationTime, "[" + std::to_string(iterations) + "]: Simulation with max. global dt " + std::to_string(maxTimeStepWidth) + " at time"
      );

      // Update simulation time with time step width
      simulationTime += maxTimeStepWidth;
      iterations++;
      progressBar.update(simulationTime);
    }

    // Print current simulation time of the output
    progressBar.clear();
    Tools::Logger::logger.printOutputTime(simulationTime);
    progressBar.update(simulationTime);

    // Write output
    // Coarse output
    if (coarse > 0)
    {
      std::cout << "\nSecond coarse Block during simulation" << std::endl;
      // Calculate how many full groups (groups of size coarse) are needed, and then how many cells are left over that will be bundled into a new average
      std::cout << "Groups n Stuff calculated as " << groupsX << ";" << restX << ";" << groupsY << ";" << restY << std::endl;
      // average the values in the arrays
      writer.writeTimeStep
      (coarseArray(waveBlock->getWaterHeight(), coarse, waveBlock->getNx(), waveBlock->getNy(), groupsX, restX, groupsY, restY),
       coarseArray(waveBlock->getDischargeHu(), coarse, waveBlock->getNx(), waveBlock->getNy(), groupsX, restX, groupsY, restY),
       coarseArray(waveBlock->getDischargeHv(), coarse, waveBlock->getNx(), waveBlock->getNy(), groupsX, restX, groupsY, restY),
       simulationTime);
    }
    else
    {
      writer.writeTimeStep(waveBlock->getWaterHeight(), waveBlock->getDischargeHu(), waveBlock->getDischargeHv(), simulationTime);
    }
  }

  progressBar.clear();
  Tools::Logger::logger.printStatisticsMessage();
  Tools::Logger::logger.printTime("CPU", "CPU Time");
  Tools::Logger::logger.printTime("CPU-Communication", "CPU + Communication Time");
  Tools::Logger::logger.getDefaultOutputStream(
  ) << "Average time per Cell: "
    << Tools::Logger::logger.getTime("CPU") / (numberOfGridCellsX * numberOfGridCellsY) << " seconds" << std::endl;
  Tools::Logger::logger.getDefaultOutputStream() << "Average time per Iteration: " << Tools::Logger::logger.getTime("CPU") / iterations << " seconds" << std::endl;
  Tools::Logger::logger.printWallClockTime(time(NULL));
  Tools::Logger::logger.printIterationsDone(iterations);
  // print number of threads
#ifdef ENABLE_OPENMP
  Tools::Logger::logger.getDefaultOutputStream() << "Number of threads: " << omp_get_max_threads() << std::endl;
#endif

  Tools::Logger::logger.printFinishMessage();

  delete waveBlock;
  delete[] checkPoints;

  return EXIT_SUCCESS;
  }
}