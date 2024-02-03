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
* A simple progress bar using stdout
*/

#include "WarningSystem.h"

Tools::WarningSystem::WarningSystem(bool used) {
  this->used = used;
  this->alarmed = false;
}

Tools::WarningSystem::WarningSystem(int destinationX, int destinationY, double threshold) {
  this->destinationX = destinationX;
  this->destinationY = destinationY;
  this->originalLevel = 0;
  this->threshold = threshold;
  this->used = true;
  this->alarmed = false;
}

void Tools::WarningSystem::setOriginalLevel(double newLevel) {
  if(used) {
    originalLevel = newLevel;
  }
}

void Tools::WarningSystem::setThreshold(double newThreshold) {
  this->threshold = newThreshold;
}

bool Tools::WarningSystem::update(double waterHeight) {
  if(used) {
    if(std::abs(originalLevel - waterHeight) >= threshold) {
      Tools::Logger::logger.printAlarm(std::abs(originalLevel - waterHeight));
      alarmed = true;
    } else {
      Tools::Logger::logger.printSafeLevel(std::abs(originalLevel - waterHeight));
    }
    if(alarmed && std::abs(originalLevel - waterHeight) < threshold) {
      std::cout << "The water level is back to normal" << std::endl;
      return true;
    }
  }
  return false;
}
