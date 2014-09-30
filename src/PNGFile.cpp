#include <include/PNGFile.h>

PNGFile::PNGFile(string file) {
  Mat originalMatrix = imread(file);
  Mat splittedChannels[3];
  split(originalMatrix, splittedChannels);
  this->dataMatrix = splittedChannels[1];
  // Obtenemos la informacion de las cabeceras del archivo
  this->width  = this->dataMatrix.rows;
  this->height = this->dataMatrix.cols;
  this->corner = new QPoint(0,0);
  this->actualDirection = 0;
  this->actualFilter = 0;
  this->windowSize = 3;
  this->intensity = 256;
}

void PNGFile::calcCoocurrence(int x, int y, int w, int h) {
  int direction[4][2] = {{0,1},{-1,1},{-1,0},{-1,-1}};
  int wSize = int(floor(this->windowSize / 2));

  Mat *matrix = 0;
  matrix = new Mat[4];
  this->coocMatrix = new Mat[4];
  selPixelsCount = new int[4];
  for (int i = 0; i < 4; i++) {
    selPixelsCount[i] = 0;
    matrix[i] = Mat::zeros(this->intensity, this->intensity, CV_8UC1);
    this->coocMatrix[i] = Mat::zeros(this->intensity, this->intensity, CV_8UC1);
  }

  // Calculamos Matriz de Co-ocurrencia para las direcciones (0, 45, 90, 135)
  int near_ = 0;
  for (int dir = 0; dir < 4; dir++) {
    for (int i = (x + wSize); i < ((x + w) - (wSize)); i++) {
      for (int j = (y + wSize); j < ((y + h) - (wSize)); j++) {
        int actual = this->dataMatrix.at<int>(i, j);
        near_ = this->dataMatrix.at<int>((i + (direction[dir][0] * wSize)), (j + (direction[dir][1] * wSize)));
        matrix[dir].at<int>(actual, near_) = matrix[dir].at<int>(actual, near_) + 1;
      }
    }
  }

  // Se traspone y se suma con las otras direcciones (180, 225, 270, 315)
  for (int dir = 0; dir < 4; dir++) {
    for (int i = 0; i < this->intensity; i++) {
      for (int j = 0; j < this->intensity; j++)
        this->coocMatrix[dir].at<int>(i, j) = matrix[dir].at<int>(i, j) + matrix[dir].at<int>(j, i);
      for (int j = 0; j < this->intensity; j++)
        this->selPixelsCount[dir] += this->coocMatrix[dir].at<int>(i, j);
    }
  }

  if (matrix != 0)
    for (int i = 0; i < 4; i++)
      matrix[i].~Mat();
  delete[] matrix;
}

void PNGFile::calcNormalized() {
  this->normMatrix = 0;
  this->normMatrix = new double **[4];

  // Calculamos la normalizacion
  for (int i = 0; i < 4; i++) {
    this->normMatrix[i] = new double *[this->intensity];
    for (int j = 0; j < this->intensity; j++) {
      this->normMatrix[i][j] = new double[this->intensity];
      for (int k = 0; k < this->intensity; k++)
        this->normMatrix[i][j][k] = double(this->coocMatrix[i].at<double>(j, k) / selPixelsCount[i]);
    }
  }
  if (this->coocMatrix != 0)
    for (int i = 0; i < 4; i++)
      this->coocMatrix[i].~Mat();
  delete[] this->coocMatrix;
  this->coocMatrix = 0;
}

void PNGFile::makeSelection (int x, int y, int w, int h) {
  this->corner->setX(x);
  this->corner->setY(y);
  this->selWidth  = w - x;
  this->selHeight = h - y;
  this->selPixelsCount = new int[4];
  for (int i = 0; i < 4; i++)
    selPixelsCount[i] = 0;
}

void PNGFile::applyFilter () {
  this->calcCoocurrence(this->corner->x(), this->corner->y(), this->selWidth, this->selHeight);
  this->calcNormalized();
  //if      (actualFilter==0) filterValue = homogenateFilter(actualDirection);
  //else if (actualFilter==1) filterValue = contrastFilter(actualDirection);
  //else if (actualFilter==2) filterValue = dissimilarFilter(actualDirection);
  //else if (actualFilter==3) filterValue = glcmFilter(actualDirection);
  //else if (actualFilter==4) filterValue = standardDeviationFilter(actualDirection);
  //else if (actualFilter==5) filterValue = entropyFilter(actualDirection);
  //else if (actualFilter==6) filterValue = correlationFilter(actualDirection);
  //else if (actualFilter==7) filterValue = asmFilter(actualDirection);
  //this->eraseData();
}

PNGFile::~PNGFile() {
  this->dataMatrix.~Mat();
}
