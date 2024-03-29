#pragma once
#include <valarray>
#include <vector>

#include "../Tools/RealType.hpp"

namespace Solvers {

  class FWaveSolver {
  public:
    /**
     * Compute net updates for the cell on the left/right side of the edge.
     *
     * @param hLeft height on the left side of the cell.
     * @param hRight height on the right side of the cell.
     * @param huLeft momentum on the left side of the cell.
     * @param huRight momentum on the right side of the cell.
     * @param bLeft bathymetry on the left side of the cell. (currently ignored)
     * @param bRight bathymetry on the right side of the cell. (currently ignored)
     *
     * @param o_hUpdateLeft return value for the left net-update for the height.
     * @param o_hUpdateRight return value for the right net-update for the height.
     * @param o_huUpdateLeft return value for the left net-update for the momentum.
     * @param o_huUpdateRight return value for the right net-update for the momentum.
     * @param o_maxWaveSpeed return value for the maximum wave speed.
     */
    void computeNetUpdates(
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
    );

    /**
     * Calculation of the Eigenvalues for the given problem, using the formula shown on the worksheet
     * formula: λ1 = u - sqrt(g*h) and λ2 = u + sqrt(g*h)
     * if both eigenvalues are negative, the right eigenvalue is set to 0
     * if both eigenvalues are positive, the left eigenvalue is set to 0
     * @param leftState The state of the left cell (h, hu)
     * @param rightState The state of the right cell (h, hu)
     * @return eigenvalues: a vector containing the eigenvalues {λ1, λ2}
     */
    std::pair<RealType, RealType> calculateEigenvalues(std::pair<RealType, RealType> leftState, std::pair<RealType, RealType> rightState);

    /**
     * Calculates the net-update of the cell to the left / right taking the p coefficient alphaP, the p Eigenvector rP,
     * and multiplying them according to the formula on the worksheet:
     * for height: alpha * 1
     * for momentum: alpha * eigenvalue
     * @param alphas The coefficient Alpha for the given calculation
     * @param eigenvalues The vector containing the Eigenvalues, which is needed to create the Eigenvectors
     * @return update: The net update A-DeltaQ or A+DeltaQ
     */
    std::pair<std::pair<RealType, RealType>, std::pair<RealType, RealType>> calculateNetUpdate(std::pair<RealType, RealType> alphas, std::pair<RealType, RealType> eigenvalues);

    /**
     *This method calculates the Alpha-Factors needed for the net-update by multiplying the inverted of the matrix built
     * by the eigenvectors, and the deltaFlux coming for the difference of the left and right state of the flux function
     * @param eigenvalues The vector containing the Eigenvalues, which is needed to create the Eigenvectors
     * @param leftState The leftState, used to calculate the DeltaFlux (h, hu)
     * @param rightState The rightState, used to calculate the DeltaFlux (h, hu)
     * @param effectOfBathymetry The previously calculated effect of the Bathymetry for the current Flux jump
     * @return alphas: A vector containg both Alpha-values
     */
    std::pair<RealType, RealType> calculateAlphas(
      std::pair<RealType, RealType> eigenvalues, std::pair<RealType, RealType> leftState, std::pair<RealType, RealType> rightState, std::pair<RealType, RealType> effectOfBathymetry
    );

    /**
     * Calculates the value hRoe(ql,qr) using the formula shown on the worksheet
     *  (hleft + hright) / 2
     * @param leftState The state of the left cell
     * @param rightState The state of the right cell
     * @return hRoe: the calculated value
     */
    RealType calculateHRoe(std::pair<RealType, RealType> leftState, std::pair<RealType, RealType> rightState);

    /**
     * Calculation of the value uRoe(ql,qr) using the formula shown on the worksheet
     * (uLeft * sqrt(hLeft) + uRight * sqrt(hRight)) / (sqrt(hLeft) + sqrt(hRight))
     * @param leftState The state of the left cell
     * @param rightState The state of the right cell
     * @return uRoe: the calculated value
     */
    RealType calculateURoe(std::pair<RealType, RealType> leftState, std::pair<RealType, RealType> rightState);

    /**
     * Substracts the given vectorValues (used to create deltaF)
     * @param leftState The first operand, the left state of the water-cell
     * @param rightState The second operand, the right state of the water-cell
     * @param leftBathymetry The third operand, the left bathymetry of the water-cell
     * @param rightBathymetry The fourth operand, the right bathymetry of the water-cell
     * @return The calculated effect of bathymetry
     */
    std::pair<RealType, RealType> calculateEffectOfBathymetry(
      std::pair<RealType, RealType> leftState, std::pair<RealType, RealType> rightState, RealType leftBathymetry, RealType rightBathymetry
    );

  private:
    // Gravity constant
    const RealType g = 9.81;

    /**
     * Evaluates the flux-function vector for a given state using the formula of the worksheet
     * @param state The state of the cell given as a vector {h, hu}
     * @return flux: The vector containing the new values f = [hu, hu^2 + 1/2*gh^2]
     */
    std::pair<RealType, RealType> evaluateFluxFunction(std::pair<RealType, RealType> state);

    /**
     * Substracts the given vectorValues (used to create deltaF)
     * @param f1 The first operand for the subtraction
     * @param f2 The seconde operand for the subtraction (f1 - f2)
     * @return The result of the subtraction
     */
    std::pair<RealType, RealType> subtractVectors(std::pair<RealType, RealType> f1, std::pair<RealType, RealType> f2);

    /**
     * Inverts the given 2x2 matrix using the formula for inverting a 2x2 matrix
     * @param matrix The matrix to be inverted (defined as a vector of vectors) with det(matrix) != 0
     * @return The inverted matrix
     */
    std::pair<std::pair<RealType, RealType>, std::pair<RealType, RealType>> invert2x2Matrix(std::pair<std::pair<RealType, RealType>, std::pair<RealType, RealType>> matrix);

    /**
     * Multiplies the 2x2 Matrix M to the Vector v (also 2 values) using the order M*v
     * @param matrix The matrix to be multiplied
     * @param vector The vector the matrix gets multiplied with
     * @return result: The 2x2 vector resulting from the multiplication
     */
    std::pair<RealType, RealType> matirxVectorMultiplication(std::vector<std::pair<RealType, RealType>> matrix, std::pair<RealType, RealType> vector);
  };
} // namespace Solvers