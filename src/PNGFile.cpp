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
  this->minWidthHeight = min(this->width, this->height);
  this->offsetWidth = round((this->width - this->minWidthHeight) / 2);
  this->offsetHeight = round((this->height - this->minWidthHeight) / 2);
  this->dataMatrix = new int*[this->width];
  for (int i = 0; i < (this->width - 0); i++) {
    this->dataMatrix[i] = new int[this->height];
    for (int j = 0; j < (this->height - 0); j++)
      this->dataMatrix[i][j] = (int)originalMatrix.data[(j * this->width * channels) + ((i * channels) + qMax(0, 1))];
  }
  this->mayRecalculate = true;
  this->normalizationMatrix = 0;
  this->filterValue = 0;
  this->corner = new QPoint(0,0);
  this->actualDirection = 0;
  this->windowSize = 3;
  this->pixelsSelectionCount = 0;
  this->intensity = 256;
  this->badgeSize = 1;
  this->started = false;
  originalMatrix.~Mat();
}

void PNGFile::startDataPlaceholders () {
  this->pixelsSelectionCount = new int*[this->badgeSize];
  this->normalizationMatrix = new double***[this->badgeSize];
  this->filterValue = new double*[this->badgeSize];
  for (int b = 0; b < this->badgeSize; b++) {
    this->filterValue[b] = new double[9];
    for (int i = 0; i < 9; i++) {
      this->filterValue[b][i] = -9999;
    }
    this->initPixelsSelectionCount (b);
    this->initNormalizationMatrix(b);
  }
  this->mayRecalculate = true;
}

void PNGFile::initNormalizationMatrix (int b) {
  this->normalizationMatrix[b] = 0;
  this->normalizationMatrix[b] = new double **[4];
  for (int d = 0; d < 4; d++) {
    this->pixelsSelectionCount[b][d] = 0;
    this->normalizationMatrix[b][d] = new double *[this->intensity];
    for (int i = 0; i < this->intensity; i++) {
      this->normalizationMatrix[b][d][i] = new double[intensity];
      for (int j = 0; j < this->intensity; j++) {
        this->normalizationMatrix[b][d][i][j] = 0;
      }
    }
  }
}

void PNGFile::resetPixelsSelectionCount (int b) {
  this->pixelsSelectionCount[b] = 0;
  this->pixelsSelectionCount[b] = new int[4];
  for (int d = 0; d < 4; d++) {
    this->pixelsSelectionCount[b][d] = 0;
  }
}

void PNGFile::initPixelsSelectionCount (int b) {
  this->pixelsSelectionCount[b] = 0;
  this->pixelsSelectionCount[b] = new int[4];
  this->resetPixelsSelectionCount (b);
}

void PNGFile::deleteNormalizationMatrix (int b) {
  try {
    if (this->normalizationMatrix[b] != 0) {
      for (int d = 0; d < 4; d++) {
        for (int i = 0; i < intensity; i++) {
          delete[] this->normalizationMatrix[b][d][i];
        }
        delete[] this->normalizationMatrix[b][d];
      }
      delete[] this->normalizationMatrix[b];
    }
  } catch (Exception e) {
  }
}

void PNGFile::deletePixelsSelectionCount (int b) {
  try {
    if (this->pixelsSelectionCount[b] != 0) {
      delete[] this->pixelsSelectionCount[b];
    }
  } catch (Exception e) {
  }
}

int *** PNGFile::calcCoocurrence(int x, int y, int w, int h, int b) {
  int directionsA[4][2] = {{1,0},{1,1},{0,1},{-1,1}};
  int directionsB[4][2] = {{-1,1},{1,1},{-1,-1},{1,-1}};
  int directions[4][2];
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 2; j++) {
      directions[i][j] = (this->pathType == 0) ? directionsA[i][j] : directionsB[i][j];
    }
  }
  int wSize = int((this->windowSize - 1) / 2);

  int ***matrix = 0;
  int ***coocurrencesMatrix = 0;

  matrix = new int **[4];
  coocurrencesMatrix = new int **[4];
  int totalDirections = 4;
  if (this->pathType == 1) {
    totalDirections = 1;
  }
  int d = totalDirections - 1;
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
  for (d = 0; d < totalDirections; d++) {
    for (int j = y; j < (y + h - wSize); j++) {
      for (int i = x; i < (x + w - wSize); i++) {
        int referencePixel = 0;
        int rI = i + wSize;
        int rJ = j + wSize;
        int nearPixel = 0;
        int nI = rI + (directions[d][0] * wSize);
        int nJ = rJ + (directions[d][1] * wSize);
        referencePixel = this->dataMatrix[rI][rJ];
        nearPixel = this->dataMatrix[nI][nJ];
        matrix[d][referencePixel][nearPixel]++;
      }
    }
  }

  // Se traspone y se suma con las otras direcciones (180, 225, 270, 315)

  for (d = 0; d < totalDirections; d++)
    for (int i = 0; i < this->intensity; i++)
      for (int j = 0; j < this->intensity; j++)
        coocurrencesMatrix[d][j][i] = matrix[d][i][j] + matrix[d][j][i];

  if (matrix != 0) {
    for (d = 0; d < totalDirections; d++) {
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
  this->resetPixelsSelectionCount (b);
  // Conteo de píxeles
  for (int d = 0; d < 4; d++)
    for (int i = 0; i < this->intensity; i++)
      for (int j = 0; j < this->intensity; j++)
        this->pixelsSelectionCount[b][d] += coocurrencesMatrix[d][i][j];
  // Normalización
  for (int d = 0; d < 4; d++) {
    for (int i = 0; i < this->intensity; i++) {
      for (int j = 0; j < this->intensity; j++) {
        this->normalizationMatrix[b][d][i][j] = double(double(coocurrencesMatrix[d][i][j]) / this->pixelsSelectionCount[b][d]);
      }
    }
  }
  this->mayRecalculate = false;
  this->started = true;
  if (coocurrencesMatrix != 0) {
    for (int d = 0; d < 4; d++) {
      for (int i = 0; i < intensity; i++) {
        delete[] coocurrencesMatrix[d][i];
      }
      delete[] coocurrencesMatrix[d];
    }
    delete[] coocurrencesMatrix;
    coocurrencesMatrix = 0;
  }
}

void PNGFile::makeSelection (int x, int y, int w, int h) {
  this->corner->setX(qMax(x, 0));
  this->corner->setY(qMax(y, 0));
  this->selWidth  = w - x;
  this->selHeight = h - y;
  mayRecalculate = true;
}

void PNGFile::applyFilter (int actualFilter) {
  int badgesCount = this->pathType == 0 ? 1 : 4;
  if (this->mayRecalculate) {
    for (int b = 0; b < badgesCount; b++) {
      int ***coocMatrix = this->calcCoocurrence(this->corner->x(), this->corner->y(), this->selWidth, this->selHeight, b);
      this->calcNormalized(coocMatrix, b);
    }
  }
  for (int b = 0; b < badgesCount; b++) {
    switch (actualFilter) {
      case 0:
        this->filterValue[b][actualFilter] = HomogeneizeFilter::apply(this->normalizationMatrix[b], this->actualDirection, this->intensity);
        break;
      case 1:
        this->filterValue[b][actualFilter] = ContrastFilter::apply(this->normalizationMatrix[b], this->actualDirection, this->intensity);
        break;
      case 2:
        this->filterValue[b][actualFilter] = DissimilarityFilter::apply(this->normalizationMatrix[b], this->actualDirection, this->intensity);
        break;
      case 3:
        this->filterValue[b][actualFilter] = GLCMMediaFilter::apply(this->normalizationMatrix[b], this->actualDirection, this->intensity);
        break;
      case 4:
        this->filterValue[b][actualFilter] = StandartDeviationFilter::apply(this->normalizationMatrix[b], this->actualDirection, this->intensity);
        break;
      case 5:
        this->filterValue[b][actualFilter] = EntropyFilter::apply(this->normalizationMatrix[b], this->actualDirection, this->intensity);
        break;
      case 6:
        this->filterValue[b][actualFilter] = CorrelationFilter::apply(this->normalizationMatrix[b], this->actualDirection, this->intensity);
        break;
      case 7:
        this->filterValue[b][actualFilter] = ASMFilter::apply(this->normalizationMatrix[b], this->actualDirection, this->intensity);
        break;
      case 8:
        this->filterValue[b][actualFilter] = UniformityFilter::apply(this->normalizationMatrix[b], this->actualDirection, this->intensity);
        break;
    }
  }
}

PNGFile::~PNGFile() {
  for (int b = 0; b < this->badgeSize; b++) {
    this->deleteNormalizationMatrix (b);
    this->deletePixelsSelectionCount (b);
  }
  delete[] this->normalizationMatrix;
  delete[] this->pixelsSelectionCount;
  try {
    if (this->dataMatrix != 0) {
      for (int i = 0; i < this->width; i++) {
        delete[] this->dataMatrix[i];
      }
      delete[] this->dataMatrix;
    }
  } catch (Exception e) {
  }
}
