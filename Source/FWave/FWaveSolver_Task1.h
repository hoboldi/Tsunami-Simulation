#pragma once
#include <vector>

#include "Tools/RealType.hpp"

namespace FWave {

  class FWaveSolver_Task1 {
  public:
    /*The main method doin the calculation and solving the InitialVlaueProblem given the parameters.
     * Parameters:
     * [in]      leftState: The state of the cell to the left consisting of {height, momentum}
     * [in]      rightState: The state of the cell to the right consisting of {height, momentum}
     * [in/out]  updateLeft: a RealType pointer which will be filled with the net-update of the left cell
     * [in/out]  updateRight: a RealType pointer which will be filled with the net-update of the right cell
     * [in/out]  eigenvalueLeft: a RealType pointer which will be filled with the first Eigenvalue of the current cell
     * [in/out]  eigenvalueRight: a double pointer which will be filled with the seconde Eigenvalue of the current cell
     * [out]     rounded: An integer showing wether the correct Eigenvalues have been returned (0) or one had to be
     * rounded (1)
     */
    int IVFSolver(
      std::vector<RealType>  leftState,
      std::vector<RealType>  rightState,
      std::vector<RealType>* updateLeft,
      std::vector<RealType>* updateRight,
      RealType leftBathymetry,
      RealType rightBathymetry,
      RealType*              eigenvalueLeft,
      RealType*              eigenvalueRight
    );

    /*Calculation of the Eigenvalues for the given problem, using the formula shown on the worksheet
     * Parameters:
     * [in]  uRoe: The value calculated with the calculateURoe method
     * [in]  hRoe: The value calculated with the calculateHRoe method
     * [out] eigenvalues: a vector containing the eigenvalues {λ1, λ2}
     */
    std::vector<RealType> calculateEigenvalues(std::vector<RealType> leftState, std::vector<RealType> rightState);

    /*Calculates the net-update of the cell to the left / right taking the p coefficient alphaP, the p Eigenvector rP,
     * and multiplying them accoring to the formula on the worksheet Parameters: [in]  alphaP: The coefficient Alpha for
     * the given calculation [in]  rP: the Eigenvector {1, λp} [out] update: The net update A-DeltaQ or A+DeltaQ
     */
    std::vector<std::vector<RealType>> calculateNetUpdate(
      std::vector<RealType> alphas, std::vector<RealType> eigenvalues, std::vector<RealType> effectOfBathymetry
    );

    /*This method calculates the Alpha-Factors needed for the net-update by multiplying the inverted of the matrix built
     * by the eigenvectors, and the deltaFlux Parameters: [in]  eigenvalues: The vector containing the Eigenvalues,
     * which is needed to create the Eigenvectors [in]  leftState: The leftState, used to calculate the DeltaFlux [in]
     * rightState: The rightState, used to calculate the DeltaFlux [out] alphas: A vector containg both Alpha-values
     */
    std::vector<RealType> calculateAlphas(
      std::vector<RealType> eigenvalues, std::vector<RealType> leftState, std::vector<RealType> rightState
    );

    /*Calculation of the value hRow(ql,qr) using the formula shown on the worksheet
     * Parameters:
     * [in]  leftState: The state of the left cell
     * [in]  rightState: The state of the right cell
     * [out] hRoe: the calculated value
     */
    RealType calculateHRoe(std::vector<RealType> leftState, std::vector<RealType> rightState);

    /*Calculation of the value uRoe(ql,qr) using the formula shown on the worksheet
     * Parameters:
     * [in]  leftState: The state of the left cell
     * [in]  rightState: The state of the right cell
     * [out] uRoe: the calculated value
     */
    RealType calculateURoe(std::vector<RealType> leftState, std::vector<RealType> rightState);
    /*
     * TODO
     */
    std::vector<RealType> calculateEffectOfBathymetry(std::vector<RealType> leftState, std::vector<RealType> rightState, RealType leftBathymetry, RealType rightBathymetry, );
  private:
    // Gravity constant
    const RealType g = 9.81;

    /*Creates the flux-fucntion vector for a given state using the formule of the worksheet
     * Parameters:
     * [in]  state: The state of the cell given as a vector {h, hu}
     * [out] flux: The vector containing the new values f = [hu, hu^2 + 1/2*gh^2]
     */
    std::vector<RealType> createFluxFunction(std::vector<RealType> state);

    /*Substracts the given vectorValues (used to create deltaF)
     * Parameters:
     * [in]  f1: The first operand for the subtraction
     * [in]  f2: The seconde operand for the subtraction (f1 - f2)
     * [out] result: The result of the subtraction
     */
    std::vector<RealType> subtractVectors(std::vector<RealType> f1, std::vector<RealType> f2);

    /*Inverts the given 2x2 matrix using the formula given on the worksheet
     * Parameters:
     * [in]  matrix: The matrix to be inverted
     * [out] result: The inverted matrix
     */
    std::vector<std::vector<RealType>> invert2x2Matrix(std::vector<std::vector<RealType>> matrix);

  };
} // namespace FWave