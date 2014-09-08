#ifndef FilePGM_H
#define FilePGM_H
#define HAVE_CONFIG_H 1
#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmimgle/dcmimage.h>

#include <fstream>
#include <iostream>
#include <cmath>
#include <cassert>
#include <QMainWindow>
#include <QFile>
#include <QDataStream>
#include <QTextStream>
#include <QList>
#include <QDir>
#include <QtDebug>
#include <QComboBox>
#include <QPushButton>
#include <QPoint>

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

class FilePGM
{

public:
    FilePGM();
    ~FilePGM();

    int width;
    int height;
    int actualDirection;
    int actualFilter;
    int intensity;
    int selWidth;
    int selHeight;
    int windowSize;
    int *selPixelsCount;
    int **intMatrix;
    int ***coocurrences;
    double ***normalized;
    double filterValue;

    QString version;
    QString comment;
    QFile *pgmFile;
    QPoint *corner;

    DicomImage *imageDICOM;

    void loadFile (QString file);
    void readProperties ();
    void initialize ();
    void makeSelection (int x, int y, int w, int h);
    void applyFilter ();
    void createMatrix (int w, int h, QFile *file);
    void calcCoocurrence (int x, int y, int w, int h);
    void calcNormalized ();
    double homogenateFilter (int direction);
    double contrastFilter (int direction);
    double dissimilarFilter (int direction);
    double glcmFilter (int direction);
    double entropyFilter (int direction);
    double asmFilter (int direction);
    double uniformityFilter (int direction);
    double media (int direction, int index, bool invertIndex);
    double standardDeviationFilter (int direction);
    double correlationFilter (int direction);
    void eraseData();
    void cleanAll();

    bool loadDCM (const string &nombreImagenDICOM, const string &nombreImagenPGM);
};

#endif
// FilePGM_H
