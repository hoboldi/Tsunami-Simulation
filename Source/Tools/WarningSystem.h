/**
* @file
* This file is part of SWE.
*
* @author Sebastian Rettenberger (rettenbs AT in.tum.de,
* http://www5.in.tum.de/wiki/index.php/Sebastian_Rettenberger,_M.Sc.)
*
* @section LICENSE
*
* SWE is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* SWE is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with SWE.  If not, see <http://www.gnu.org/licenses/>.
*
*
* @section DESCRIPTION
*
* A Warning System
*/
#include <cmath>
#include "Logger.hpp"

#pragma once

namespace Tools {

  /**
     * @class WarningSystem
     * @brief Represents a Warning System for the chosen destination coordinates.
     *
     * This class provides functionality for a warning system that monitors water levels
     * and triggers warnings based on user-defined thresholds.
   */

 class WarningSystem {
 private:

   double biggestDifference = 0;
   double biggestTime = 0;
   int destinationX; /**< X-coordinate of the warning system's destination. */
   int destinationY; /**< Y-coordinate of the warning system's destination. */
   double originalLevel; /**< Original water level used as a reference for warnings. */
   double threshold;  /**< Threshold value for triggering warnings. */
   bool used;  /**< Flag indicating if the warning system is actively used. */
   bool alarmed; /**< Flag indicating if the warning system has been triggered. */

 public:

   /**
         * @brief Default constructor for WarningSystem.
    */
   WarningSystem();

   /**
         * @brief Parameterized constructor for WarningSystem.
         *
         * @param destinationX X-coordinate of the warning system's destination.
         * @param destinationY Y-coordinate of the warning system's destination.
    */
   WarningSystem(int destinationX, int destinationY);

   /**
         * @brief Default destructor for WarningSystem.
    */
   ~WarningSystem() = default;


   /**
         * @brief Set the threshold value for triggering warnings.
         *
         * @param newThreshold The new threshold value.
    */
   void setThreshold(double newThreshold);

   /**
         * @brief Set the original water level used as a reference for warnings.
         *
         * @param newLevel The new original water level.
    */
   void setOriginalLevel(double newLevel);

   /**
         * @brief Set the flag indicating if the warning system is actively used.
         *
         * @param newUsed The new value for the "used" flag.
    */
   void setUsed(bool newUsed);

   /**
         * @brief Update the warning system based on the current water height.
         *
         * @param waterHeight The current water height to be checked against the threshold and original level.
         * @return True if the warning was already triggered and water level is safe again.
    */
   bool update(double waterHeight, double time);
 };

} // namespace Tools
