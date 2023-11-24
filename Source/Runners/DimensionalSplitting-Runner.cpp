
#include <string>

#include "Blocks/DimensionalSplitting.h"
#include "BoundaryEdge.hpp"
#include "Readers/NetCDFReader.h"
#include "Scenarios/RadialDamBreakScenario.hpp"
#include "Scenarios/TsunamiScenario.h"
#include "Tools/Args.hpp"
#include "Tools/Logger.hpp"
#include "Tools/ProgressBar.hpp"
#include "Writers/Writer.hpp"


int main(int argc, char** argv) {
  Tools::Args args;
  args.addOption("grid-size-x", 'x', "Number of cells in x direction");
  args.addOption("grid-size-y", 'y', "Number of cells in y direction");
  args.addOption("output-basepath", 'o', "Output base file name");
  args.addOption("number-of-checkpoints", 'n', "Number of checkpoints to write output files");
  args.addOption("simulation-time", 't', "Simulation time in seconds");
  args.addOption("boundary-conditions", 'b', "Set Boundary Conditions represented by an 4 digit Integer of 0s and 1s. (1: Outflow, 2: Wall).\n First Digit: Left Boundary\n Second Digit: Right Boundary\n Third Digit: Bottom Boundary\n Fourth Digit: Top Boundary");

  Tools::Args::Result ret = args.parse(argc, argv);
  if (ret == Tools::Args::Result::Help) {
    return 0;
  }
  if (ret == Tools::Args::Result::Error) {
    return 1;
  }

  // Create the scenario
  Scenarios::TsunamiScenario scenario;
  scenario.readScenario("chile_gebco_usgs_2000m_bath.nc", "chile_gebco_usgs_2000m_displ.nc");

  int         numberOfGridCellsX = args.getArgument<int>("grid-size-x", 10);
  int         numberOfGridCellsY = args.getArgument<int>("grid-size-y", 10);
  std::string baseName           = args.getArgument<std::string>("output-basepath", "SWE");
  int numberOfCheckPoints = args.getArgument<int>("number-of-checkpoints", 20); //! Number of checkpoints for visualization (at each checkpoint in time, an output file is written).
  double endSimulationTime  = args.getArgument<double>("simulation-time", 10);
  int   boundaryConditions = args.getArgument<int>("boundary-conditions", 1111);  // Default is 0: Outflow

  //Ihhgitt!!
  if(boundaryConditions == 1111 || boundaryConditions == 1112 || boundaryConditions == 1121 || boundaryConditions == 1122 || boundaryConditions == 1211 || boundaryConditions == 1212 || boundaryConditions == 1221 || boundaryConditions == 1222 || boundaryConditions == 2111 || boundaryConditions == 2112 || boundaryConditions == 2121 || boundaryConditions == 2122 || boundaryConditions == 2211 || boundaryConditions == 2212 || boundaryConditions == 2221 || boundaryConditions == 2222) {
    scenario.setBoundaryType(boundaryConditions);
  } else {
    std::cout << "Boundary conditions invalid!" << std::endl;
    return 1;
  }

  if(endSimulationTime >= 0){
    scenario.setEndSimulationTime(endSimulationTime);
  } else {
    std::cout << "Simulation time must be positive" << std::endl;
    return 1;
  }

  Tools::Logger::logger.printWelcomeMessage();

  // Print information about the grid
  Tools::Logger::logger.printNumberOfCells(numberOfGridCellsX, numberOfGridCellsY);


  // Compute the size of a single cell
  RealType cellSizeX = (scenario.getBoundaryPos(BoundaryEdge::Right) - scenario.getBoundaryPos(BoundaryEdge::Left)) / numberOfGridCellsX;
  RealType cellSizeY = (scenario.getBoundaryPos(BoundaryEdge::Top) - scenario.getBoundaryPos(BoundaryEdge::Bottom)) / numberOfGridCellsY;

  auto waveBlock = new Blocks::DimensionalSplitting(numberOfGridCellsX, numberOfGridCellsY, cellSizeX, cellSizeY);
  waveBlock->initialiseScenario(0, 0, scenario);

  double* checkPoints = new double[numberOfCheckPoints + 1];

  for (int cp = 0; cp <= numberOfCheckPoints; cp++) {
    checkPoints[cp] = cp * (endSimulationTime / numberOfCheckPoints);
  }

  Writers::BoundarySize boundarySize = {{1, 1, 1, 1}};

  auto writer = Writers::Writer::createWriterInstance(
    baseName,
    waveBlock->getBathymetry(),
    boundarySize,
    numberOfGridCellsX,
    numberOfGridCellsY,
    cellSizeX,
    cellSizeY,
    0,
    0,
    scenario.getBoundaryPos(BoundaryEdge::Left),
    scenario.getBoundaryPos(BoundaryEdge::Bottom),
    1
  );
  Tools::ProgressBar progressBar(endSimulationTime);
  progressBar.update(0.0);
  writer->writeTimeStep(waveBlock->getWaterHeight(), waveBlock->getDischargeHu(), waveBlock->getDischargeHv(), 0.0);

  Tools::Logger::logger.printStartMessage();
  Tools::Logger::logger.initWallClockTime(time(NULL));

  double simulationTime = 0.0;
  progressBar.update(simulationTime);

  unsigned int iterations = 0;

  // Loop over checkpoints
  for (int cp = 1; cp <= numberOfCheckPoints; cp++) {
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
    writer->writeTimeStep(waveBlock->getWaterHeight(), waveBlock->getDischargeHu(), waveBlock->getDischargeHv(), simulationTime);
  }

  progressBar.clear();
  Tools::Logger::logger.printStatisticsMessage();
  Tools::Logger::logger.printTime("CPU", "CPU Time");
  Tools::Logger::logger.printTime("CPU-Communication", "CPU + Communication Time");
  Tools::Logger::logger.printWallClockTime(time(NULL));
  Tools::Logger::logger.printIterationsDone(iterations);

  Tools::Logger::logger.printFinishMessage();

  delete waveBlock;
  delete[] checkPoints;

  return EXIT_SUCCESS;
}