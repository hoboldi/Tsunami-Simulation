#include "Float2D.hpp"
#include "RealType.hpp"
namespace Tools {
  class Coarse {
  public:
    static Tools::Float2D<RealType> coarseArray(const Tools::Float2D<RealType>& array, int coarse, int nx, int ny, int groupsX, int restX, int groupsY, int restY) {
      int addX = 0;
      if (restX != 0) {
        addX++;
      }
      int addY = 0;
      if (restY != 0) {
        addY++;
      }

      RealType averagedValue = 0;

      Tools::Float2D<RealType> tempStorageX(groupsX + addX + 2, ny + 2, true);
      // Average the values in x Direction
      for (int y = 1; y <= ny; y++) {
        averagedValue = 0;
        for (int x = 1; x <= groupsX; x++) {
          averagedValue = 0;
          for (int i = 1; i <= coarse; i++) {
            averagedValue += array[(x - 1) * coarse + i][y];
          }
          averagedValue = averagedValue / coarse;
          tempStorageX[x][y] = averagedValue;
        }
        averagedValue = 0;
        // Collect the remaining restX cells into one
        if (addX != 0) {
          averagedValue = 0;
          for (int i = 1; i <= restX; i++) {
            averagedValue += array[groupsX * coarse + i][y];
          }
          averagedValue = averagedValue / restX;
          tempStorageX[(groupsX + addX)][y] = averagedValue;
        }
      }

      // Average the values in y direction
      Tools::Float2D<RealType> tempStorageY(groupsX + addX + 2, groupsY + addY + 2, true);
      for (int x = 1; x <= groupsX + addX; x++) {
        averagedValue = 0;
        for (int y = 1; y <= groupsY; y++) {
          averagedValue = 0;
          for (int i = 1; i <= coarse; i++) {
            averagedValue += tempStorageX[x][(y - 1) * coarse + i];
          }
          averagedValue = averagedValue / coarse;
          tempStorageY[x][y] = averagedValue;
        }
        // Collect the remaining restY rows below
        if (addY != 0) {
          averagedValue = 0;
          for (int i = 1; i <= restY; i++) {
            averagedValue += tempStorageX[x][groupsY * coarse + i];
          }
          averagedValue = averagedValue / restY;
          tempStorageY[x][groupsY + addY] = averagedValue;
        }
      }
      return tempStorageY;
    }
  };
} // namespace Tools
