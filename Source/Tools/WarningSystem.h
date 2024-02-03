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

 class WarningSystem {
 private:

   int destinationX;
   int destinationY;
   double originalLevel;
   double threshold;
   bool used;

 public:
   WarningSystem();
   WarningSystem(int destinationX, int destinationY);
   ~WarningSystem() = default;

   void setThreshold(double newThreshold);
   void setOriginalLevel(double newLevel);
   void setUsed(bool newUsed);

   void update(double waterHeight);
 };

} // namespace Tools
