/* This file contains the method definition for the Subcritical-Flow Scenario
* Defined in Sheet Task 2.1
*/

#pragma once

#include "Tools/RealType.hpp"


namespace Scenarios
{
    class SubcriticalFlow
    {
        /** Number of cells */
        const unsigned int size_;

    public:
        SubcriticalFlow(unsigned int size);
        ~SubcriticalFlow() = default;

        const RealType g = 9.81;
        /**
         * @brief Get the Size of a cell in the current state
         * 
         * @return [out] The calculated Cell Size
         */
        RealType getCellSize() const;

        /**
         * @brief Get the Height at the cell at pos
         * 
         * @param [in] pos: The position of the cell 
         * @return [out] The calculated Height
         */
        RealType getHeight(int pos) const;

        /**
         * @brief Get the Momentum of the cell at pos
         * 
         * @param [in] pos: The position of the cell 
         * @return [out] The calculated Momentum
         */
        RealType getMomentum(int pos) const;

        /**
         * @brief Get the Bathymetry of the cell at pos
         * 
         * @param [in] pos: The position of the cell 
         * @return [out] The calculated Bathymetry
         */
        RealType getBathymetry(int pos) const;

        /**
         * @brief Get the Froud Number of the cell at pos
         * 
         * @param [in] pos: The position of the cell 
         * @return [out] The calculated Fround number
         */
        RealType getFroudNumber(int pos) const;

        /**
         * @brief Get the maximum Froud number for the current field at time 0. If there is no cell, both values will be filled with "-1"
         * 
         * @param [in/out] value: A RealyType pointer that will contain the maximum Froud number at the end
         * @param [in/out] position: A RealType pointer that will contain the position where the maximum Froud number is located
         */
        void getMaximumFroudNumber(RealType* value, RealType* position) const;

        /**
         * @brief A metod used for internal rounding, to avoid floating-point execeptions
         * 
         * @param [in] value: Value to be rounded
         * @param [in] precision: The prescision needed, f.e. for 3 digits after comma, use 0.001
         * @return [out] The calculated Fround number
         */
        double round_to(RealType value, double precision = 0.000001) const;
    };
}