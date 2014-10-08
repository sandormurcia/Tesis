#ifndef GLCMMEDIAFILTER_H
#define GLCMMEDIAFILTER_H

#include <qmath.h>

class GLCMMediaFilter
{
  public:
    static double apply(double ***m, int d, int intensity) {
      double result = 0;
      for (int i = 0; i < intensity; i++) {
        for (int j = 0; j < intensity; j++) {
          result = result + m[d][i][j] * i;
        }
      }
      return result;
    }
};

#endif // GLCMMEDIAFILTER_H
