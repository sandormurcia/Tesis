#ifndef ASMFILTER_H
#define ASMFILTER_H

#include <qmath.h>

class ASMFilter
{
  public:
    static double apply(double ***m, int d, int intensity) {
      double result = 0;
      for (int i = 0; i < intensity; i++) {
        for (int j = 0; j < intensity; j++) {
          result += (qPow(m[d][i][j], 2.0));
        }
      }
      return result;
    }
};

#endif // ASMFILTER_H
