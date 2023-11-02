#include "FWaveSolver_Task1.h"

#include <cassert>
#include <cmath>
#include <iostream>

using namespace FWave;

int FWaveSolver_Task1::IVFSolver(
  std::vector<RealType>  leftState,
  std::vector<RealType>  rightState,
  std::vector<RealType>* updateLeft,
  std::vector<RealType>* updateRight,
  RealType leftBathymetry,
  RealType rightBathymetry,
  RealType*              eigenvalueLeft,
  RealType*              eigenvalueRight
) {
  int rounded = 0;

  std::vector<RealType> eigenvalues = calculateEigenvalues(leftState, rightState);
  eigenvalueLeft                    = &eigenvalues[0];
  eigenvalueRight                   = &eigenvalues[1];
  if (eigenvalues[0] < 0 && eigenvalues[1] < 0) {
    *eigenvalueRight = 0;
    rounded          = 1;
  } else if (eigenvalues[0] > 0 && eigenvalues[1] > 0) {
    *eigenvalueLeft = 0;
    rounded         = 1;
  }
  std::vector<RealType> effectOfBathymetry = calculateEffectOfBathymetry(leftState,rightState,leftBathymetry,rightBathymetry);
  std::vector<RealType>              alphas     = calculateAlphas(eigenvalues, leftState, rightState);
  std::vector<std::vector<RealType>> netUpdates = calculateNetUpdate(alphas, eigenvalues, effectOfBathymetry);
  updateLeft                                    = &netUpdates[0];
  updateRight                                   = &netUpdates[1];
  return rounded;
}

RealType FWaveSolver_Task1::calculateHRoe(std::vector<RealType> leftState, std::vector<RealType> rightState) {
  return (leftState[0] + rightState[0]) / 2;
}

RealType FWaveSolver_Task1::calculateURoe(std::vector<RealType> leftState, std::vector<RealType> rightState) {
  RealType uL = leftState[1] / leftState[0];
  RealType uR = rightState[1] / rightState[0];
  // Heights cant be negative
  assert(leftState[0] >= 0);
  assert(rightState[0] >= 0);
  return (uL * sqrt(leftState[0]) + uR * sqrt(rightState[0])) / (sqrt(leftState[0]) + sqrt(rightState[0]));
}

std::vector<RealType> FWaveSolver_Task1::calculateEigenvalues(
  std::vector<RealType> leftState, std::vector<RealType> rightState
) {
  RealType              hRoe        = calculateHRoe(leftState, rightState);
  RealType              uRoe        = calculateURoe(leftState, rightState);
  std::vector<RealType> eigenvalues = {0, 0};
  eigenvalues[0]                    = uRoe - sqrt(g * hRoe);
  eigenvalues[1]                    = uRoe + sqrt(g * hRoe);
  return eigenvalues;
}

std::vector<RealType> FWaveSolver_Task1::createFluxFunction(std::vector<RealType> state) {
  std::vector<RealType> result = {0, 0};
  result[0]                    = state[1];
  result[1]                    = (state[1] * state[1]) / state[0] + 0.5 * g * state[0] * state[0];
  return result;
}

std::vector<std::vector<RealType>> FWaveSolver_Task1::invert2x2Matrix(std::vector<std::vector<RealType>> matrix) {
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

std::vector<RealType> FWaveSolver_Task1::subtractVectors(std::vector<RealType> first, std::vector<RealType> second) {
  first[0] -= second[0];
  first[1] -= second[1];
  return first;
}

std::vector<RealType> FWaveSolver_Task1::calculateAlphas(
  std::vector<RealType> eigenvalues, std::vector<RealType> leftState, std::vector<RealType> rightState
) {
  std::vector<RealType> deltaFlux = subtractVectors(createFluxFunction(rightState), createFluxFunction(leftState));
  std::vector<RealType> alphas    = {0, 0};
  std::vector<std::vector<RealType>> matrix = {{1, 1}, eigenvalues};
  matrix                                    = invert2x2Matrix(matrix);
  alphas[0]                                 = matrix[0][0] * deltaFlux[0] + matrix[0][1] * deltaFlux[1];
  alphas[1]                                 = matrix[1][0] * deltaFlux[0] + matrix[1][1] * deltaFlux[1];
  return alphas;
}

std::vector<RealType> FWaveSolver_Task1::calculateEffectOfBathymetry(
  std::vector<RealType> leftState, std::vector<RealType> rightState, RealType leftBathymetry, RealType rightBathymetry
  )
{
  std::vector<RealType> effectOfBathymetry = {0,0};
  effectOfBathymetry[1] = -g *(rightBathymetry - leftBathymetry) * (leftState [0] + rightState[0])/2;
  return effectOfBathymetry;
}

  std::vector<std::vector<RealType>> FWaveSolver_Task1::calculateNetUpdate(
  std::vector<RealType> alphas, std::vector<RealType> eigenvalues, std::vector<RealType> effectOfBathymetry
) {
  // Eigenvalues cant be zero
  assert(eigenvalues[0] != 0 && eigenvalues[1] != 0);

  std::vector<RealType> netUpdateLeft  = {0, 0};
  std::vector<RealType> netUpdateRight = {0, 0};

  if (eigenvalues[0] < 0) {
    netUpdateLeft = {alphas[0] * 1, alphas[0] * eigenvalues[0]};
  } else {
    netUpdateRight = {alphas[0] * 1, alphas[0] * eigenvalues[0]};
  }
  if (eigenvalues[1] < 0) {
    netUpdateLeft[0] += alphas[1] * 1;
    netUpdateLeft[1] += alphas[1] * eigenvalues[1];
  } else {
    netUpdateRight[0] += alphas[1] * 1;
    netUpdateRight[1] += alphas[1] * eigenvalues[1];
  }
  netUpdateLeft = subtractVectors(netUpdateLeft,effectOfBathymetry);
  netUpdateRight = subtractVectors(netUpdateRight,effectOfBathymetry);

  return {netUpdateLeft, netUpdateRight};
}


