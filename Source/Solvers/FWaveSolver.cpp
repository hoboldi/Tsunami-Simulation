#include "FWaveSolver.h"

#include <cassert>
#include <iostream>

void Solvers::FWaveSolver::computeNetUpdates(
  const RealType &hLeft,
  const RealType &hRight,
  const RealType &huLeft,
  const RealType &huRight,
  const RealType &bLeft,
  const RealType &bRight,
  RealType &o_hUpdateLeft,
  RealType &o_hUpdateRight,
  RealType &o_huUpdateLeft,
  RealType &o_huUpdateRight,
  RealType &o_maxWaveSpeed
) {
  std::pair<RealType, RealType> leftState = {hLeft, huLeft};
  std::pair<RealType, RealType> rightState = {hRight, huRight};

  RealType bLeft_ = bLeft;
  RealType bRight_ = bRight;
  //Left cell dry, right cell wet
  if (hLeft <= 0) {
    leftState.first = rightState.first;
    leftState.second = -rightState.second;
    bLeft_ = bRight;
  }
  //Left cell wet, right cell dry
  else if (hRight <= 0) {
    rightState.first = leftState.first;
    rightState.second = -leftState.second;
    bRight_ = bLeft;
  } else if (hLeft <= 0 && hRight <= 0) {
    o_hUpdateLeft = 0;
    o_huUpdateLeft = 0;
    o_hUpdateRight = 0;
    o_huUpdateRight = 0;
    o_maxWaveSpeed = 0;
    return;
  }

  RealType hRoe = calculateHRoe(leftState, rightState);
  RealType uRoe = calculateURoe(leftState, rightState);

  std::pair<RealType, RealType> eigenvalues = calculateEigenvalues(leftState, rightState);
  std::pair<RealType, RealType> effectOfBathymetry = calculateEffectOfBathymetry(leftState, rightState, bLeft_, bRight_);
  std::pair<RealType, RealType> alphas = calculateAlphas(eigenvalues, leftState, rightState, effectOfBathymetry);
  std::pair<std::pair<RealType, RealType>, std::pair<RealType, RealType>> netUpdates = calculateNetUpdate(alphas, eigenvalues);
  if (eigenvalues.first < 0 && eigenvalues.second < 0) {
    eigenvalues.first = 0;
  } else if (eigenvalues.first > 0 && eigenvalues.second > 0) {
    eigenvalues.second = 0;
  }
  o_hUpdateLeft = netUpdates.first.first;
  o_huUpdateLeft = netUpdates.first.second;
  o_hUpdateRight = netUpdates.second.first;
  o_huUpdateRight = netUpdates.second.second;
  o_maxWaveSpeed = std::max(std::fabs(eigenvalues.first), std::fabs(eigenvalues.second));
  //Left cell dry, right cell wet
  if (hLeft <= 0) {
    o_hUpdateLeft = 0;
    o_huUpdateLeft = 0;
  }
  //Left cell wet, right cell dry
  else if (hRight <= 0) {
    o_hUpdateRight = 0;
    o_huUpdateRight = 0;
  }
}

RealType Solvers::FWaveSolver::calculateHRoe(std::pair<RealType, RealType> leftState, std::pair<RealType, RealType> rightState) {
  return (leftState.first + rightState.first) / 2;
}

RealType Solvers::FWaveSolver::calculateURoe(std::pair<RealType, RealType> leftState, std::pair<RealType, RealType> rightState) {
  RealType uL = leftState.second / leftState.first;
  RealType uR = rightState.second / rightState.first;
  return (uL * sqrt(leftState.first) + uR * sqrt(rightState.first)) / (sqrt(leftState.first) + sqrt(rightState.first));
}

std::pair<RealType, RealType> Solvers::FWaveSolver::calculateEigenvalues(
  std::pair<RealType, RealType> leftState, std::pair<RealType, RealType> rightState
) {
  RealType hRoe = calculateHRoe(leftState, rightState);
  RealType uRoe = calculateURoe(leftState, rightState);
  std::pair<RealType, RealType> eigenvalues = {0, 0};
  eigenvalues.first = uRoe - sqrt(g * hRoe);
  eigenvalues.second = uRoe + sqrt(g * hRoe);
  return eigenvalues;
}

std::pair<RealType, RealType> Solvers::FWaveSolver::evaluateFluxFunction(std::pair<RealType, RealType> state) {
  std::pair<RealType, RealType> result = {0, 0};
  RealType u = state.second / state.first;
  result.first = state.second;
  result.second = state.second * u + 0.5 * g * state.first * state.first;
  return result;
}

std::pair<std::pair<RealType, RealType>, std::pair<RealType, RealType>> Solvers::FWaveSolver::invert2x2Matrix(std::pair<std::pair<RealType, RealType>, std::pair<RealType, RealType>> matrix) {
  std::pair<std::pair<RealType, RealType>, std::pair<RealType, RealType>> inverseMatrix;
  inverseMatrix.first.first = matrix.second.second / (matrix.first.first * matrix.second.second - matrix.first.second * matrix.second.first);
  inverseMatrix.first.second = -matrix.first.second / (matrix.first.first * matrix.second.second - matrix.first.second * matrix.second.first);
  inverseMatrix.second.first = -matrix.second.first / (matrix.first.first * matrix.second.second - matrix.first.second * matrix.second.first);
  inverseMatrix.second.second = matrix.first.first / (matrix.first.first * matrix.second.second - matrix.first.second * matrix.second.first);
  return inverseMatrix;
}

std::pair<RealType, RealType> Solvers::FWaveSolver::subtractVectors(std::pair<RealType, RealType> first, std::pair<RealType, RealType> second) {
  first.first -= second.first;
  first.second -= second.second;
  return first;
}

std::pair<RealType, RealType> Solvers::FWaveSolver::calculateAlphas(
  std::pair<RealType, RealType> eigenvalues, std::pair<RealType, RealType> leftState, std::pair<RealType, RealType> rightState,
  std::pair<RealType, RealType> effectOfBathymetry) {
  std::pair<RealType, RealType> deltaFlux = subtractVectors(evaluateFluxFunction(rightState),
                                                            evaluateFluxFunction(leftState));
  deltaFlux = subtractVectors(deltaFlux, effectOfBathymetry);
  std::pair<RealType, RealType> alphas = {0, 0};
  std::pair<std::pair<RealType, RealType>, std::pair<RealType, RealType>> matrix = {{1, 1}, eigenvalues};
  matrix = invert2x2Matrix(matrix);
  alphas.first = matrix.first.first * deltaFlux.first + matrix.first.second * deltaFlux.second;
  alphas.second = matrix.second.first * deltaFlux.first + matrix.second.second * deltaFlux.second;
  return alphas;
}

std::pair<RealType, RealType> Solvers::FWaveSolver::calculateEffectOfBathymetry(
  std::pair<RealType, RealType> leftState, std::pair<RealType, RealType> rightState, RealType leftBathymetry,
  RealType rightBathymetry
) {
  std::pair<RealType, RealType> effectOfBathymetry = {0, 0};
  effectOfBathymetry.second = -g * (rightBathymetry - leftBathymetry) * (leftState.first + rightState.first) / (2.0);
  return effectOfBathymetry;
}

std::pair<std::pair<RealType, RealType>, std::pair<RealType, RealType>> Solvers::FWaveSolver::calculateNetUpdate(
  std::pair<RealType, RealType> alphas, std::pair<RealType, RealType> eigenvalues
) {

  std::pair<RealType, RealType> netUpdateLeft = {0, 0};
  std::pair<RealType, RealType> netUpdateRight = {0, 0};

  if (eigenvalues.first < 0) {
    netUpdateLeft = {alphas.first * 1, alphas.first * eigenvalues.first};
  } else if (eigenvalues.first > 0) {
    netUpdateRight = {alphas.first * 1, alphas.first * eigenvalues.first};
  }
  if (eigenvalues.second < 0) {
    netUpdateLeft.first += alphas.second * 1;
    netUpdateLeft.second += alphas.second * eigenvalues.second;
  } else if (eigenvalues.second > 0) {
    netUpdateRight.first += alphas.second * 1;
    netUpdateRight.second += alphas.second * eigenvalues.second;
  }

  return {netUpdateLeft, netUpdateRight};
}
