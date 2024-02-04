
// #include <format>
#include <string>

#include "Blocks/DimensionalSplitting.h"
#include "BoundaryEdge.hpp"
#include "Readers/NetCDFReader.h"
#include "Scenarios/ArtificialTsunamiScenario.h"
#include "Scenarios/CheckpointScenario.h"
#include "Scenarios/FileScenario.h"
#include "Scenarios/RadialDamBreakScenario.hpp"
#include "Scenarios/TsunamiScenario.h"
#include "Scenarios/WorldScenario.h"
#include "Tools/Args.hpp"
#include "Tools/Coarse.h"
#include "Tools/WarningSystem.h"
#include "Tools/Logger.hpp"
#include "Tools/ProgressBar.hpp"
#include "Writers/NetCDFWriter.hpp"
#include "Writers/Writer.hpp"
#ifdef ENABLE_OPENMP
#include <omp.h>
#endif

void print2DArray(RealType array[], int dimX, int dimY) {
  for (int y = 0; y < dimY; y++) {
    for (int x = 0; x < dimX; x++) {
      std::cout << array[dimX * y + x] << "\t";
    }
    std::cout << "\n";
  }
}

void printFloat2D(const Tools::Float2D<RealType>& array, int dimX, int dimY) {
  std::cout << "\nPrinting Float2D:" << std::endl;
  for (int y = 0; y < dimY + 2; y++) {
    for (int x = 0; x < dimX + 2; x++) {
      std::cout << array[x][y] << "\t";
    }
    std::cout << "\n";
  }
}

/**
 * @brief A method to convert an entered magnitude on the richter scale (R) to the magnitude in moment notation (M)
 * This is necessary to calculate further, and uses the approximative equations to convert via the logarithmic energy of the earthquake
 * - Richter: Log E = 11.8 + 1.5 R
 * - Moment Magnitude: Log E = 5.24 + 1.44 M
 * 
 * @param richter [in] The magnitude on the richter scale
 * @return moment [out] The magnitude on the moment-magnitude scale
 */
RealType richterToMagnitude(RealType richter)
{
  RealType energy = 11.8 + (1.5 * richter);
  return (energy - 5.24) / 1.44;
}

/**
 * @brief This method is used to convert the human entered coordinates in longitude (geographic coordinates) to their equivalent used on our map
 * The P(0,0) for us is in the bottom left, the max Point in the top right
 * Longitude goes from -180° (west) to 180° degrees (east)
 * 
 * @param maxX [in] the maximum x used in our internal system
 * @param enteredX [in] the x entered by the user
 * @return rx [out] the x we computed, used for our future calculations
 */
RealType convertEnteredXtoMappedX(RealType maxX, RealType enteredX)
{
  // We set the start to the middle of our map, which is also the base-case for entered x = 0
  RealType rx = maxX / 2;
  //Which means that the value is in westwards direction, so in the first half of our map
  if (enteredX < 0 && enteredX >= -180)
  {
    RealType movingFactor = maxX / 360;
    rx -= movingFactor * enteredX * (-1);
  }
  //Right at the middle
  else if (enteredX > 0 && enteredX <= 180)
  {
    RealType movingFactor = maxX / 360;
    rx += movingFactor * enteredX;
  }
  else if (enteredX != 0)
  {
    std::cout << "Invalid coordinates! The longitude x goes only from -180° (West) to 180° (East)! Setting x value to 0" << std::endl;
    rx = 0;
  }
  std::cout << "Converted " << enteredX << " to " << rx << std::endl;
  return rx;
}

/**
 * @brief This method is used to convert the human entered coordinates in lattitude (geographic coordinates) to their equivalent used on our map
 * The P(0,0) for us is in the bottom left, the max Point in the top right
 * Longitude goes from -90° (south) to 90 degrees (north)
 * 
 * @param maxY [in] the maximum x used in our internal system
 * @param enteredY [in] the x entered by the user
 * @return rY [out] the x we computed, used for our future calculations
 */
RealType convertEnteredYtoMappedY(RealType maxY, RealType enteredY)
{
  // We set the start to the middle of our map, which is also the base-case for entered x = 0
  RealType rY = maxY / 2;
  //Which means that the value is in southwards direction, so in the bottom half of our map
  if (enteredY < 0 && enteredY >= -90)
  {
    RealType movingFactor = maxY / 180;
    rY -= movingFactor * enteredY * (-1);
  }
  //Right at the middle
  else if (enteredY > 0 && enteredY <= 180)
  {
    RealType movingFactor = maxY / 360;
    rY += movingFactor * enteredY;
  }
  else if (enteredY != 0)
  {
    std::cout << "Invalid coordinates! The lattitude y goes only from -90° (South) to 90° (North)! Setting y value to 0" << std::endl;
    rY = 0;
  }
  std::cout << "Converted " << enteredY << " to " << rY << std::endl;
  return rY;
}

/**
 * @brief This method can be used to convert an array of sizes nx / ny to a coarsed version with the averaged values according to Sheet 4 Task 5
 *
 * @param array [in] The array to be coarsed
 * @param coarse [in] The parameter that defines how many cells should be added into one
 * @param nx [in] The dimension in X direction of the origin array
 * @param ny [in] The dimension in Y direction of the origin array
 * @param groupsX [in] The amount of full groups that can be formed in X direction -> nx / coarse as full integer rounded down
 * @param restX [in] The amount of leftover cells in X direction that don't form a full group
 * @param groupsY [in] The amount of full groups that can be formed in Y direction -> ny / coarse as full integer rounded down
 * @param restY [in] The amount of leftover cells in Y direction that don't form a full group
 * @return [out] The array of coarsed values which can then be printed
 */
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
  args.addOption("magnitude", 'm', "The moment-megnitude of the eartquake");
  args.addOption("richter-scale", 'r', "The magnitude on the richter scale, this should not be used as it will be subject to many approximation errors");
  args.addOption("destinationX", 'd', "The X geographical coordinate of the destination city. Negative values mean degrees west, positive degrees east");
  args.addOption("destinationY", 's', " The y geographical coordinate of the destination city. Negative values mean degrees south, positive degrees north");
  args.addOption("epicenterX", 'e', "The X geographical coordinate of the epicenter. Negative values mean degrees west, positive degrees east");
  args.addOption("epicenterY", 'f', "The Y geographical coordinate of the epicenter. Negative values mean degrees south, positive degrees north");
  args.addOption("threshold", 'l', "The threshold variable represents the critical water level change that, when surpassed, triggers a warning in the tsunami detection system");

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
  int         coarse             = args.getArgument<int>("coarse", 0);                 // Default is 0 if no coarse should be used
  RealType    magnitude          = args.getArgument<RealType>("magnitude", 0);
  RealType    richter            = args.getArgument<RealType>("richter-scale", 0);
  RealType    destinationXUser      = args.getArgument<RealType>("destinationX", 0);
  RealType    destinationYUser      = args.getArgument<RealType>("destinationY", 0); 
  RealType    epicenterXUser       = args.getArgument<RealType>("epicenterX", 0); 
  RealType    epicenterYUser       = args.getArgument<RealType>("epicenterY", 0);
  double      threshold          =  args.getArgument<double>("threshold", 1);

  //Error message if the user wants to use the WorldScenario, but forgets a value
  if (epicenterXUser != 0 || epicenterYUser != 0 || destinationXUser != 0 || destinationYUser != 0 || magnitude != 0 || richter != 0)
  {
      if (magnitude == 0 && richter == 0)
      {
        std::cout << "Error, no magnitude entered in neither format. Please use the -m option to enter your desired moment-megnitude!";
      }
      if (magnitude == 0 && richter != 0)
      {
        magnitude = richterToMagnitude(richter);
      }
  }

int destinationX = 0;
int destinationY = 0;
int epicenterX = 0;
int epicenterY = 0;

  if (magnitude != 0)
  {
    if (magnitude < 6.51)
    {
      std::cout << "Magnitude too small, can't compute Tsunami wave" << std::endl;
      return 5;
    }

    epicenterX = convertEnteredXtoMappedX(numberOfGridCellsX, epicenterXUser);
    epicenterY = convertEnteredYtoMappedY(numberOfGridCellsY, epicenterYUser);
    destinationX = convertEnteredXtoMappedX(numberOfGridCellsX, destinationXUser);
    destinationY = convertEnteredYtoMappedY(numberOfGridCellsY, destinationYUser);

    //print the values for the user to see
    std::cout << "EpicenterX: " << epicenterX << " EpicenterY: " << epicenterY << " DestinationX: " << destinationX << " DestinationY: " << destinationY << std::endl;
  }

  std::vector<RealType> coarseHeights;
  std::vector<RealType> coarseHus;
  std::vector<RealType> coarseHvs;

  Tools::Logger::logger.printWelcomeMessage();
  Tools::WarningSystem warningSystem(false);

  // Print information about the grid
  Tools::Logger::logger.printNumberOfCells(numberOfGridCellsX, numberOfGridCellsY);
  Scenarios::Scenario* scenario;

  if (checkpointFile.empty()) {
    if (magnitude != 0)
    {
      /*replaced world scenario with the File Scenario, this si jsut for the sake of testing

      auto worldScenario = new Scenarios::WorldScenario(epicenterX, epicenterY, magnitude);
      worldScenario->readWorld("GEBCO_2023_TID.nc");
      scenario = worldScenario;
      */
      auto fileScenario = new Scenarios::FileScenario("GEBCO_2023_sub_ice_topo.nc", numberOfGridCellsX, numberOfGridCellsY, 0, epicenterX, epicenterY, magnitude);
      scenario = fileScenario;
      /*
      if(!fileScenario->checkIfEpicenterIsOnWater(epicenterX, epicenterY))
      {
        std::cout << "Test here " << fileScenario->getBathymetry(epicenterX, epicenterY) << std::endl;
        std::cout << "Epicenter set on land, aborting calculation! Restart and make sure you set it on water." << std::endl;
        return 6;
      }
      */
      warningSystem = new Tools::WarningSystem(destinationX,destinationY,threshold);
      warningSystem.setThreshold(threshold);
    }
    else
    {
      auto tsunamiScenario = new Scenarios::TsunamiScenario();
      tsunamiScenario->readScenario("chile_gebco_usgs_2000m_bath.nc", "chile_gebco_usgs_2000m_displ.nc");
      scenario = tsunamiScenario;
    }
    //auto tsunamiScenario = new Scenarios::FileScenario("GEBCO_2023_TID.nc", numberOfGridCellsX, numberOfGridCellsY, 0);
    //scenario = tsunamiScenario;
    //TO CALL PATHFINDER
    //PATHFINDER TO CALL PostEarthquake to Get new domain

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
  Tools::Logger::logger.printString("Init Waveblock");
  waveBlock->initialiseScenario(0, 0, *scenario);
  Tools::Logger::logger.printString("Init finished");

  double* checkPoints = new double[numberOfCheckPoints + 1];

  for (int cp = 0; cp <= numberOfCheckPoints; cp++) {
    checkPoints[cp] = cp * (endSimulationTime / numberOfCheckPoints);
  }

  Writers::BoundarySize boundarySize = {{1, 1, 1, 1}};
  int                   groupsX      = 0;
  int                   groupsY      = 0;
  int                   restX        = 0;
  int                   restY        = 0;
  int                   addX         = 0;
  int                   addY         = 0;
  if (coarse > 0) {
    groupsX = waveBlock->getNx() / coarse;
    restX   = waveBlock->getNx() - (groupsX * coarse);
    groupsY = waveBlock->getNy() / coarse;
    restY   = waveBlock->getNy() - (groupsY * coarse);
    addX    = (restX != 0) ? 1 : 0;
    addY    = (restY != 0) ? 1 : 0;
  }
  // bathymetry copy
  auto* bathymetry = new RealType[(waveBlock->getNx() + 2) * (waveBlock->getNy() + 2)];
  if (coarse <= 0) {

    for (int i = 0; i < (waveBlock->getNx() + 2) * (waveBlock->getNy() + 2); i++) {
      int x         = i / (waveBlock->getNx() + 2);
      int y         = i % (waveBlock->getNx() + 2);
      bathymetry[i] = waveBlock->getBathymetry()[x][y];
    }
  }
  Tools::Float2D<RealType> coarseArr(Tools::Coarse::coarseArray(waveBlock->getBathymetry(), coarse, waveBlock->getNx(), waveBlock->getNy(), groupsX, restX, groupsY, restY));

  Tools::Float2D<RealType> bathyCopy(waveBlock->getNx() + 2, waveBlock->getNy() + 2, bathymetry);
  Writers::NetCDFWriter    writer
    = checkpointFile.empty()
        ? Writers::NetCDFWriter(
          baseName,
          ((coarse <= 0) ? bathyCopy : coarseArr),
          boundarySize,
          boundaryConditions,
          ((coarse <= 0) ? numberOfGridCellsX : (groupsX + addX)),
          ((coarse <= 0) ? numberOfGridCellsY : (groupsY + addY)),
          ((coarse <= 0) ? cellSizeX : (cellSizeX * coarse)),
          ((coarse <= 0) ? cellSizeY : (cellSizeY * coarse)),
          scenario->getBoundaryPos(BoundaryEdge::Left),
          scenario->getBoundaryPos(BoundaryEdge::Bottom),
          1
        )
        : Writers::NetCDFWriter(checkpointFile, ((coarse <= 0) ? numberOfGridCellsX : (groupsX + addX)), ((coarse <= 0) ? numberOfGridCellsY : (groupsY + addY)), boundarySize, 1);
  Tools::ProgressBar progressBar(endSimulationTime);
  progressBar.update(0.0);
  if (checkpointFile.empty()) {

    // Coarse output here
    if (coarse > 0) {
      // average the values in the arrays
      Tools::Float2D<RealType> waterHeight(Tools::Coarse::coarseArray(waveBlock->getWaterHeight(), coarse, waveBlock->getNx(), waveBlock->getNy(), groupsX, restX, groupsY, restY));
      Tools::Float2D<RealType> dischargeHu(Tools::Coarse::coarseArray(waveBlock->getDischargeHu(), coarse, waveBlock->getNx(), waveBlock->getNy(), groupsX, restX, groupsY, restY));
      Tools::Float2D<RealType> dischargeHv(Tools::Coarse::coarseArray(waveBlock->getDischargeHv(), coarse, waveBlock->getNx(), waveBlock->getNy(), groupsX, restX, groupsY, restY));
      writer.writeTimeStep(waterHeight, dischargeHu, dischargeHv, 0.0);
    } else {
      writer.writeTimeStep(waveBlock->getWaterHeight(), waveBlock->getDischargeHu(), waveBlock->getDischargeHv(), 0.0);
    }
  }
  double simulationTime = scenario->getStartTime();
  progressBar.update(simulationTime);
  // skip until correct checkpoint
  int cp = 1;
  while (simulationTime > checkPoints[cp] && cp <= numberOfCheckPoints) {
    cp++;
  }

  Tools::Logger::logger.printStartMessage();
  double wallClockTime = 1;
  Tools::Logger::logger.initWallClockTime(wallClockTime);

  warningSystem.setOriginalLevel(waveBlock->getWaterHeight()[destinationX][destinationY]);

  unsigned int iterations = 0;
  // Loop over checkpoints
  for (; cp <= numberOfCheckPoints; cp++) {
    // Do time steps until next checkpoint is reached
    while (simulationTime < checkPoints[cp]) {
      // Reset CPU-Communication clock
      Tools::Logger::logger.resetClockToCurrentTime("CPU-Communication");

      // Reset the cpu clock
      Tools::Logger::logger.resetClockToCurrentTime("CPU");

#if defined(ENABLE_OPENMP)
      double start_time = omp_get_wtime();
#endif

      // Set values in ghost cells
      // waveBlock->setGhostLayer();
      // Compute numerical flux on each edge
      waveBlock->computeNumericalFluxes();
      RealType maxTimeStepWidth = waveBlock->getMaxTimeStep();
      // std::cout << "GetMaxTimestep called Value: " << maxTimeStepWidth << std::endl;
      //  Update the cell values
      waveBlock->updateUnknowns(maxTimeStepWidth);

#if defined(ENABLE_OPENMP)
      double end_time = omp_get_wtime();
      wallClockTime += end_time - start_time;
#endif

      // Update the cpu time in the logger
      Tools::Logger::logger.updateTime("CPU");
      Tools::Logger::logger.updateTime("CPU-Communication");

      // Print the current simulation time
      progressBar.clear();
      Tools::Logger::logger.printSimulationTime(
        simulationTime, "[" + std::to_string(iterations) + "]: Simulation with max. global dt " + std::to_string(maxTimeStepWidth) + " at time"
      );
      if(warningSystem.update(waveBlock->getWaterHeight()[destinationX][destinationY])){
        goto endSimulation;
      }

      // Update simulation time with time step width
      simulationTime += maxTimeStepWidth;
      iterations++;
      progressBar.update(simulationTime);
    }

    // Print current simulation time of the output
    progressBar.clear();
    Tools::Logger::logger.printOutputTime(simulationTime);
    progressBar.update(simulationTime);
    // Coarse output
    if (coarse > 0) {
      // average the values in the arrays
      Tools::Float2D<RealType> waterHeight(Tools::Coarse::coarseArray(waveBlock->getWaterHeight(), coarse, waveBlock->getNx(), waveBlock->getNy(), groupsX, restX, groupsY, restY));
      Tools::Float2D<RealType> dischargeHu(Tools::Coarse::coarseArray(waveBlock->getDischargeHu(), coarse, waveBlock->getNx(), waveBlock->getNy(), groupsX, restX, groupsY, restY));
      Tools::Float2D<RealType> dischargeHv(Tools::Coarse::coarseArray(waveBlock->getDischargeHv(), coarse, waveBlock->getNx(), waveBlock->getNy(), groupsX, restX, groupsY, restY));
      writer.writeTimeStep(waterHeight, dischargeHu, dischargeHv, simulationTime);
    } else {
      writer.writeTimeStep(waveBlock->getWaterHeight(), waveBlock->getDischargeHu(), waveBlock->getDischargeHv(), simulationTime);
    }
  }
  endSimulation:
  progressBar.clear();
  Tools::Logger::logger.printStatisticsMessage();
  Tools::Logger::logger.printTime("CPU", "CPU Time");
  Tools::Logger::logger.printTime("CPU-Communication", "CPU + Communication Time");
  Tools::Logger::logger.getDefaultOutputStream(
  ) << "Average time per Cell: "
    << Tools::Logger::logger.getTime("CPU") / (numberOfGridCellsX * numberOfGridCellsY) << " seconds" << std::endl;
  Tools::Logger::logger.getDefaultOutputStream() << "Average time per Iteration: " << Tools::Logger::logger.getTime("CPU") / iterations << " seconds" << std::endl;
  Tools::Logger::logger.printWallClockTime(wallClockTime);


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
