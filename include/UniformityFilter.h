#ifndef UNIFORMITYFILTER_H
#define UNIFORMITYFILTER_H

#include <qmath.h>

class UniformityFilter
{
  public:
    static double apply(double ***m, int d, int intensity) {
      double result = 0;
      for (int i = 0; i < intensity; i++) {
        for (int j = 0; j < intensity; j++) {
          result += (qPow(m[d][i][j], 2.0));
        }
      }
      return qSqrt(result);
    }
};

#endif // UNIFORMITYFILTER_H
