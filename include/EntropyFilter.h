#ifndef ENTROPYFILTER_H
#define ENTROPYFILTER_H

#include <qmath.h>

class EntropyFilter
{
  public:
    static double apply(double ***m, int d, int intensity) {
      double result = 0;
      for (int i = 0; i < intensity; i++) {
        for (int j = 0; j < intensity; j++) {
          if (m[d][i][j] != 0)
            result -= (m[d][i][j] * qLn(m[d][i][j]));
        }
      }
      return result;
    }
};

#endif // ENTROPYFILTER_H
