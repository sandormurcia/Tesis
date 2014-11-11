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
#include <include/EntropyFilter.h>
#include <include/CorrelationFilter.h>
#include <include/ASMFilter.h>
#include <include/UniformityFilter.h>

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
    int pathType;
    int selWidth;
    int selHeight;
    int windowSize;
    int minWidthHeight;
    int offsetWidth;
    int offsetHeight;
    int **pixelsSelectionCount;
    double **filterValue;
    bool mayRecalculate;
    bool started;

    QPoint *corner;

    int **dataMatrix;
    double ****normalizationMatrix;
    double **results;

    void makeSelection (int x, int y, int w, int h);
    void applyFilter (int actualFilter);
    void createMatrix (int w, int h, QFile *file);
    int *** calcCoocurrence (int x, int y, int w, int h);
    void calcNormalized (int ***coocurrencesMatrix, int b);
    void startDataPlaceholders ();
    void initNormalizationMatrix (int b);
    void initPixelsSelectionCount (int b);
    void resetPixelsSelectionCount (int b);
    void deleteNormalizationMatrix (int b);
    void deletePixelsSelectionCount (int b);
};

#endif
// PNGFILE_H
