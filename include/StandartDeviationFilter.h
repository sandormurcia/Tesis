#ifndef STANDARTDEVIATIONFILTER_H
#define STANDARTDEVIATIONFILTER_H

#include <qmath.h>

class StandartDeviationFilter
{
  public:
    static double media(double ***m, int d, int intensity, int index, bool invertIndex) {
      double miu = 0;
      if (invertIndex == false) {
        for (int i = 0; i < intensity; i++)
          miu += m[d][index][i];
      } else {
        for (int i = 0; i < intensity; i++)
          miu += m[d][i][index];
      }
      return (miu/qPow(intensity, 2));
    }
    static double apply(double ***m, int d, int intensity) {
      double result = 0, miu;
      for (int i = 0; i < intensity; i++) {
        miu = StandartDeviationFilter::media(m, d, intensity, i, false);
        for (int j = 0; j < intensity; j++)
          result += m[d][i][j] * qPow(double(i - miu), 2.0);
      }
      return qSqrt(result);
   }
};

#endif // STANDARTDEVIATIONFILTER_H
