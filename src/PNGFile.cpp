#include <include/PNGFile.h>

QString iats(int a[], int s) {
  QString o = "";
  for (int i = 0; i < s; i++) {
    if (a[i] > 0)
      o += QString::number(a[i]) + ",";
    else
      o += ",";
  }
  return o;
}

QString dats(double a[], int s) {
  QString o = "";
  for (int i = 0; i < s; i++) {
    if (a[i] > 0)
      o += QString::number(a[i]) + ",";
    else
      o += ",";
  }
  return o;
}

PNGFile::PNGFile(string file) {
  Mat originalMatrix = imread(file);
  // Obtenemos la informacion de las cabeceras del archivo
  this->width  = originalMatrix.cols;
  this->height = originalMatrix.rows;
  int channels = originalMatrix.channels();
  this->dataMatrix = new int*[this->width];
  for (int i = 0; i < this->width; i++) {
    this->dataMatrix[i] = new int[this->height];
    for (int j = 0; j < this->height; j++)
      this->dataMatrix[i][j] = (int)originalMatrix.data[(j * this->width * channels) + ((i * channels) + qMax(0, 1))];
  }
  this->mayRecalculate = true;
  this->normMatrix = 0;
  this->filterValue = 0;
  this->corner = new QPoint(0,0);
  this->actualDirection = 0;
  this->actualFilter = 0;
  this->pathType = 0;
  this->windowSize = 3;
  this->selPixelsCount = 0;
  this->intensity = 256;
  this->badgeSize = this->pathType == 0 ? 1 : 4;

  this->selPixelsCount = new int*[this->badgeSize];
  this->normMatrix = new double***[this->badgeSize];
  this->filterValue = new double*[this->badgeSize];
  for (int b = 0; b < this->badgeSize; b++) {
    this->filterValue[b] = new double[9];
    for (int i = 0; i < 9; i++) {
      this->filterValue[b][i] = -9999;
    }
    this->cleanNormalizationData(b);
  }
}

void PNGFile::cleanNormalizationData (int b) {
  if (this->normMatrix[b] != 0) {
    for (int d = 0; d < 4; d++) {
      for (int i = 0; i < intensity; i++)
        delete[] this->normMatrix[b][d][i];
      delete[] this->normMatrix[b][d];
    }
    delete[] this->normMatrix[b];
  }
  if (this->selPixelsCount[b] != 0) {
    delete[] this->selPixelsCount[b];
  }
  this->normMatrix[b] = 0;
  this->selPixelsCount[b] = 0;
  this->selPixelsCount[b] = new int[4];
  this->normMatrix[b] = new double **[4];
  for (int d = 0; d < 4; d++) {
    this->selPixelsCount[b][d] = 0;
    this->normMatrix[b][d] = new double *[this->intensity];
    for (int i = 0; i < this->intensity; i++) {
      this->normMatrix[b][d][i] = new double[intensity];
      for (int j = 0; j < this->intensity; j++) {
        this->normMatrix[b][d][i][j] = 0;
      }
    }
  }
}

int *** PNGFile::calcCoocurrence(int x, int y, int w, int h) {
  int direction[4][2] = {{1,0},{1,1},{0,1},{-1,1}};
  int wSize = int(floor(this->windowSize / 2));

  int ***matrix = 0;
  int ***coocurrencesMatrix = 0;

  matrix = new int **[4];
  coocurrencesMatrix = new int **[4];
  int d = 3;
  do {
    matrix[d] = new int *[this->intensity];
    coocurrencesMatrix[d] = new int *[this->intensity];
    int i = this->intensity - 1;
    do {
      matrix[d][i] = new int[this->intensity];
      coocurrencesMatrix[d][i] = new int[this->intensity];
      int j = this->intensity - 1;
      do {
        matrix[d][i][j] = 0;
        coocurrencesMatrix[d][i][j] = 0;
        j--;
      } while (j >= 0);
      i--;
    } while (i >= 0);
    d--;
  } while (d >= 0);

  // Calculamos Matriz de Co-ocurrencia para las direcciones (0, 45, 90, 135)
  int near = 0;
  for (d = 0; d < 4; d++) {
    for (int i = (x + wSize); i < ((x + w) - (wSize)); i++) {
      for (int j = (y + wSize); j < ((y + h) - (wSize)); j++) {
        int actual = this->dataMatrix[i][j];
        near = this->dataMatrix[(i + (direction[d][0] * wSize))][(j + (direction[d][1] * wSize))];
        matrix[d][actual][near]++;
      }
    }
  }

  // Se traspone y se suma con las otras direcciones (180, 225, 270, 315)

  for (d = 0; d < 4; d++)
    for (int i = 0; i < this->intensity; i++)
      for (int j = 0; j < this->intensity; j++)
        coocurrencesMatrix[d][j][i] = matrix[d][i][j] + matrix[d][j][i];

  if (matrix != 0) {
    for (d = 0; d < 4; d++) {
      for (int i = 0; i < intensity; i++)
        delete[] matrix[d][i];
      delete[] matrix[d];
    }
    delete[] matrix;
    matrix = 0;
  }
  return coocurrencesMatrix;
}

void PNGFile::calcNormalized(int ***coocurrencesMatrix, int b) {
  if (this->mayRecalculate) {
    cleanNormalizationData(b);
    // Conteo de píxeles
    for (int d = 0; d < 4; d++)
      for (int i = 0; i < this->intensity; i++)
        for (int j = 0; j < this->intensity; j++)
          this->selPixelsCount[b][d] += coocurrencesMatrix[d][i][j];
    // Normalización
    for (int d = 0; d < 4; d++) {
      for (int i = 0; i < this->intensity; i++) {
        for (int j = 0; j < this->intensity; j++) {
          this->normMatrix[b][d][i][j] = double(double(coocurrencesMatrix[d][i][j]) / this->selPixelsCount[b][d]);
        }
      }
    }
    this->mayRecalculate = false;
  }
}

void PNGFile::makeSelection (int x, int y, int w, int h) {
  this->corner->setX(qMax(x, 0));
  this->corner->setY(qMax(y, 0));
  this->selWidth  = w - x;
  this->selHeight = h - y;
  for (int i = 0; i < 4; i++)
    this->selPixelsCount[i] = 0;
  mayRecalculate = true;
}

void PNGFile::applyFilter () {
  int *** coocMatrix = this->calcCoocurrence(this->corner->x(), this->corner->y(), this->selWidth, this->selHeight);
  this->calcNormalized(coocMatrix, 0);
  switch (this->actualFilter) {
    case 0:
      this->filterValue[0][this->actualFilter] = HomogeneizeFilter::apply(this->normMatrix[0], this->actualDirection, this->intensity);
      break;
    case 1:
      this->filterValue[0][this->actualFilter] = ContrastFilter::apply(this->normMatrix[0], this->actualDirection, this->intensity);
      break;
    case 2:
      this->filterValue[0][this->actualFilter] = DissimilarityFilter::apply(this->normMatrix[0], this->actualDirection, this->intensity);
      break;
    case 3:
      this->filterValue[0][this->actualFilter] = GLCMMediaFilter::apply(this->normMatrix[0], this->actualDirection, this->intensity);
      break;
    case 4:
      this->filterValue[0][this->actualFilter] = StandartDeviationFilter::apply(this->normMatrix[0], this->actualDirection, this->intensity);
      break;
  }
  //else if (actualFilter==5) filterValue = entropyFilter(actualDirection);
  //else if (actualFilter==6) filterValue = correlationFilter(actualDirection);
  //else if (actualFilter==7) filterValue = asmFilter(actualDirection);
  //this->eraseData();
  // qDebug() << QString::number(filterValue);
}

PNGFile::~PNGFile() {
  if (this->dataMatrix != 0) {
    for (int i = 0; i < height; i++) {
      delete[] this->dataMatrix[i];
    }
    delete[] this->dataMatrix;
    this->dataMatrix = 0;
  }
}
