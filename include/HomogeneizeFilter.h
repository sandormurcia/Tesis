#ifndef HOMOGENEIZEFILTER_H
#define HOMOGENEIZEFILTER_H

#include <QDebug>
#include <include/BaseStatisticalFilter.h>

class HomogeneizeFilter : public BaseStatisticalFilter
{
  public:
    explicit HomogeneizeFilter(Mat m);
    double apply(int d);
};

#endif // HOMOGENEIZEFILTER_H
