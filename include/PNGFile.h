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

    int width;
    int height;
    int actualDirection;
    int actualFilter;
    int selWidth;
    int selHeight;
    int windowSize;
    int *selPixelsCount;
    double filterValue;

    QPoint *corner;

    Mat dataMatrix;
    Mat *coocMatrix;
    double ***normMatrix;

    void makeSelection (int x, int y, int w, int h);
    void applyFilter ();
    void createMatrix (int w, int h, QFile *file);
    void calcCoocurrence (int x, int y, int w, int h);
    void calcNormalized ();
};

#endif
// PNGFILE_H
