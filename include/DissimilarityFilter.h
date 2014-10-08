#ifndef DISSIMILARITYFILTER_H
#define DISSIMILARITYFILTER_H

#include <qmath.h>

class DissimilarityFilter
{
  public:
    static double apply(double ***m, int d, int intensity) {
      double result = 0;
      for (int i = 0; i < intensity; i++) {
        for (int j = 0; j < intensity; j++) {
          result = result + (m[d][i][j] * qAbs(i - j));
        }
      }
      return result;
    }
};


#endif // DISSIMILARITYFILTER_H
