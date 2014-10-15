#ifndef CORRELATIONFILTER_H
#define CORRELATIONFILTER_H

#include <include/StandartDeviationFilter.h>
#include <qmath.h>

class CorrelationFilter
{
  public:
    static double apply(double ***m, int d, int intensity) {
      double result = 0;
      double sD = StandartDeviationFilter::apply(m, d, intensity);
      for (int i = 0; i < intensity; i++) {
        for (int j = 0; j < intensity; j++) {
          result += m[d][i][j] * (((i - StandartDeviationFilter::media(m, d, intensity, i, false)) * (j - StandartDeviationFilter::media(m, d, intensity, j, true))) / (qSqrt(qPow(sD, 4.0))));
        }
      }
      return result;
    }
};

#endif // CORRELATIONFILTER_H
