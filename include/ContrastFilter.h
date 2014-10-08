#ifndef CONTRASTFILTER_H
#define CONTRASTFILTER_H

#include <qmath.h>

class ContrastFilter
{
  public:
    static double apply(double ***m, int d, int intensity) {
      double result = 0;
      for (int i = 0; i < intensity; i++) {
        for (int j = 0; j < intensity; j++) {
          result = result + (m[d][i][j] * pow((double)(i - j), 2.0));
        }
      }
      return result;
    }
};

#endif // CONTRASTFILTER_H
