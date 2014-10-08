#ifndef HOMOGENEIZEFILTER_H
#define HOMOGENEIZEFILTER_H

#include <qmath.h>

class HomogeneizeFilter
{
  public:
    static double apply(double ***m, int d, int intensity) {
      double result = 0;
      for (int i = 0; i < intensity; i++) {
        for (int j = 0; j < intensity; j++) {
          result += m[d][i][j]/(1.0 + pow(((double)(i - j)), 2.0));
        }
      }
      return result;
    }
};

#endif // HOMOGENEIZEFILTER_H
