#ifndef BASESTATISTICALFILTER_H
#define BASESTATISTICALFILTER_H

#include <iostream>
#include <stdio.h>
#include <sstream>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

class BaseStatisticalFilter
{
  public:
    virtual double apply(int d) = 0;
};
#endif
// BASESTATISTICALFILTER_H
