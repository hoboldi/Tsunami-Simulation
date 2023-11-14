#include "FWaveSolver.h"

#include <cassert>
#include <iostream>

void Solvers::FWaveSolver::computeNetUpdates(
  const RealType& hLeft,
  const RealType& hRight,
  const RealType& huLeft,
  const RealType& huRight,
  const RealType& bLeft,
  const RealType& bRight,
  RealType&       o_hUpdateLeft,
  RealType&       o_hUpdateRight,
  RealType&       o_huUpdateLeft,
  RealType&       o_huUpdateRight,
  RealType&       o_maxWaveSpeed
) {
  std::vector<RealType> leftState  = {hLeft, huLeft};
  std::vector<RealType> rightState = {hRight, huRight};

  //Left cell dry, right cell wet
  if(bLeft >= 0 && bRight < 0) {
    hLeft_       = hRight_;
    huLeft_      = -huRight_;
    bLeft_       = bRight_;
  }
  //Left cell wet, right cell dry
  else if(bLeft < 0 && bRight >= 0) {
    hRight_      = hLeft_;
    huRight_     = -huLeft_;
    bRight_      = bLeft_;
  }
  else if(bLeft >= 0 && bRight >= 0) {
    hLeft_       = 0;
    huLeft_      = 0;
    bLeft_       = 0;
    hRight_      = 0;
    huRight_     = 0;
    bRight_      = 0;
  }
  else if(bLeft >= 0 && bRight >= 0) {
    o_hUpdateLeft   = 0;
    o_huUpdateLeft  = 0;
    o_hUpdateRight  = 0;
    o_huUpdateRight = 0;
    o_maxWaveSpeed  = 0;
    return;
  }

  std::vector<RealType>              eigenvalues        = calculateEigenvalues(leftState, rightState);
  std::vector<RealType>              effectOfBathymetry = calculateEffectOfBathymetry(leftState, rightState, bLeft, bRight);
  std::vector<RealType>              alphas             = calculateAlphas(eigenvalues, leftState, rightState, effectOfBathymetry);
  std::vector<std::vector<RealType>> netUpdates         = calculateNetUpdate(alphas, eigenvalues);
  if (eigenvalues[0] < 0 && eigenvalues[1] < 0) {
    eigenvalues[0] = 0;
  } else if (eigenvalues[0] > 0 && eigenvalues[1] > 0) {
    eigenvalues[1] = 0;
  }
  o_hUpdateLeft   = netUpdates[0][0];
  o_huUpdateLeft  = netUpdates[0][1];
  o_hUpdateRight  = netUpdates[1][0];
  o_huUpdateRight = netUpdates[1][1];
  o_maxWaveSpeed  = std::max(std::fabs(eigenvalues[0]), std::fabs(eigenvalues[1]));
}

RealType Solvers::FWaveSolver::calculateHRoe(std::vector<RealType> leftState, std::vector<RealType> rightState) { return (leftState[0] + rightState[0]) / 2; }

RealType Solvers::FWaveSolver::calculateURoe(std::vector<RealType> leftState, std::vector<RealType> rightState) {
  RealType uL = leftState[1] / leftState[0];
  RealType uR = rightState[1] / rightState[0];
  return (uL * sqrt(leftState[0]) + uR * sqrt(rightState[0])) / (sqrt(leftState[0]) + sqrt(rightState[0]));
}

std::vector<RealType> Solvers::FWaveSolver::calculateEigenvalues(std::vector<RealType> leftState, std::vector<RealType> rightState) {
  RealType              hRoe        = calculateHRoe(leftState, rightState);
  RealType              uRoe        = calculateURoe(leftState, rightState);
  std::vector<RealType> eigenvalues = {0, 0};
  eigenvalues[0]                    = uRoe - sqrt(g * hRoe);
  eigenvalues[1]                    = uRoe + sqrt(g * hRoe);
  return eigenvalues;
}

std::vector<RealType> Solvers::FWaveSolver::evaluateFluxFunction(std::vector<RealType> state) {
  std::vector<RealType> result = {0, 0};
  RealType              u      = state[1] / state[0];
  result[0]                    = state[1];
  result[1]                    = state[1] * u + 0.5 * g * state[0] * state[0];
  return result;
}

std::vector<std::vector<RealType>> Solvers::FWaveSolver::invert2x2Matrix(std::vector<std::vector<RealType>> matrix) {
  std::vector<std::vector<RealType>> inverseMatrix;
  inverseMatrix.resize(2);
  inverseMatrix[0].resize(2);
  inverseMatrix[1].resize(2);

  RealType determinant = matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
  // Make sure the matrix is invertible
  assert(determinant != 0);

  inverseMatrix[0][0] = matrix[1][1] / determinant;
  inverseMatrix[0][1] = -matrix[0][1] / determinant;
  inverseMatrix[1][0] = -matrix[1][0] / determinant;
  inverseMatrix[1][1] = matrix[0][0] / determinant;

  return inverseMatrix;
}

std::vector<RealType> Solvers::FWaveSolver::subtractVectors(std::vector<RealType> first, std::vector<RealType> second) {
  first[0] -= second[0];
  first[1] -= second[1];
  return first;
}

std::vector<RealType> Solvers::FWaveSolver::calculateAlphas(
  std::vector<RealType> eigenvalues, std::vector<RealType> leftState, std::vector<RealType> rightState, std::vector<RealType> effectOfBathymetry
) {
  std::vector<RealType> deltaFlux           = subtractVectors(evaluateFluxFunction(rightState), evaluateFluxFunction(leftState));
  deltaFlux                                 = subtractVectors(deltaFlux, effectOfBathymetry);
  std::vector<RealType>              alphas = {0, 0};
  std::vector<std::vector<RealType>> matrix = {{1, 1}, eigenvalues};
  matrix                                    = invert2x2Matrix(matrix);
  alphas[0]                                 = matrix[0][0] * deltaFlux[0] + matrix[0][1] * deltaFlux[1];
  alphas[1]                                 = matrix[1][0] * deltaFlux[0] + matrix[1][1] * deltaFlux[1];
  return alphas;
}

std::vector<RealType> Solvers::FWaveSolver::calculateEffectOfBathymetry(
  std::vector<RealType> leftState, std::vector<RealType> rightState, RealType leftBathymetry, RealType rightBathymetry
) {
  std::vector<RealType> effectOfBathymetry = {0, 0};
  effectOfBathymetry[1]                    = -g * (rightBathymetry - leftBathymetry) * (leftState[0] + rightState[0]) / (2.0);
  return effectOfBathymetry;
}


std::vector<std::vector<RealType>> Solvers::FWaveSolver::calculateNetUpdate(std::vector<RealType> alphas, std::vector<RealType> eigenvalues) {

  std::vector<RealType> netUpdateLeft  = {0, 0};
  std::vector<RealType> netUpdateRight = {0, 0};

  if (eigenvalues[0] < 0) {
    netUpdateLeft = {alphas[0] * 1, alphas[0] * eigenvalues[0]};
  } else if (eigenvalues[0] > 0) {
    netUpdateRight = {alphas[0] * 1, alphas[0] * eigenvalues[0]};
  }
  if (eigenvalues[1] < 0) {
    netUpdateLeft[0] += alphas[1] * 1;
    netUpdateLeft[1] += alphas[1] * eigenvalues[1];
  } else if (eigenvalues[1] > 0) {
    netUpdateRight[0] += alphas[1] * 1;
    netUpdateRight[1] += alphas[1] * eigenvalues[1];
  }

  return {netUpdateLeft, netUpdateRight};
}
