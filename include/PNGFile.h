#ifndef PNGFILE_H
#define PNGFILE_H

#include <fstream>
#include <iostream>
#include <cmath>
#include <cassert>
#include <QFile>
#include <QDataStream>
#include <QTextStream>
#include <QList>
#include <QDir>
#include <QtDebug>
#include <QPoint>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <include/HomogeneizeFilter.h>
#include <include/ContrastFilter.h>
#include <include/DissimilarityFilter.h>
#include <include/GLCMMediaFilter.h>
#include <include/StandartDeviationFilter.h>

QT_BEGIN_NAMESPACE

class QDebug;
class QFile;
class QString;
class QDataStream;
class QTextStream;
class QDir;
class QPoint;
QT_END_NAMESPACE

using namespace std;
using namespace cv;

class PNGFile
{
  private:
    int intensity;

  public:
    PNGFile(string file);
    ~PNGFile();

    int badgeSize;
    int width;
    int height;
    int actualDirection;
    int actualFilter;
    int pathType;
    int selWidth;
    int selHeight;
    int windowSize;
    int **selPixelsCount;
    double **filterValue;
    bool mayRecalculate;

    QPoint *corner;

    int **dataMatrix;
    double ****normMatrix;
    double **results;

    void makeSelection (int x, int y, int w, int h);
    void applyFilter ();
    void createMatrix (int w, int h, QFile *file);
    int *** calcCoocurrence (int x, int y, int w, int h);
    void calcNormalized (int ***coocurrencesMatrix, int b);
    void cleanNormalizationData (int b);
};

#endif
// PNGFILE_H
