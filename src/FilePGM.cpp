#include "FilePGM.h"

FilePGM::FilePGM () {
}

void FilePGM::loadFile (QString fileName) {
  // Inicializamos la conexion al archivo
  pgmFile = new QFile(fileName);
  pgmFile->open(QIODevice::ReadOnly);
}

void FilePGM::readProperties () {
  // Obtenemos la informacion de las cabeceras del archivo
  version = pgmFile->readLine().trimmed();
  comment = pgmFile->readLine().trimmed();
  QList<QString> dimensions = ((QString) pgmFile->readLine()).trimmed().split(' ');
  width  = dimensions.at(0).toInt();
  height = dimensions.at(1).toInt();
  corner = new QPoint(0,0);
  actualDirection = 0;
  actualFilter = 0;
  windowSize = 3;
  intensity = pgmFile->readLine().trimmed().toInt() + 1;
}

void FilePGM::initialize () {
  readProperties();
  createMatrix(width, height, pgmFile);
  pgmFile->~QFile();
}

void FilePGM::makeSelection (int x, int y, int w, int h) {
  corner->setX(x);
  corner->setY(y);
  selWidth  = w - x;
  selHeight = h - y;
  selPixelsCount = new int[4];
  for (int i = 0; i < 4; i++)
    selPixelsCount[i] = 0;
}

void FilePGM::applyFilter () {
  calcCoocurrence(corner->x(), corner->y(), selWidth, selHeight);
  calcNormalized();
  if      (actualFilter==0) filterValue = homogenateFilter(actualDirection);
  else if (actualFilter==1) filterValue = contrastFilter(actualDirection);
  else if (actualFilter==2) filterValue = dissimilarFilter(actualDirection);
  else if (actualFilter==3) filterValue = glcmFilter(actualDirection);
  else if (actualFilter==4) filterValue = standardDeviationFilter(actualDirection);
  else if (actualFilter==5) filterValue = entropyFilter(actualDirection);
  else if (actualFilter==6) filterValue = correlationFilter(actualDirection);
  else if (actualFilter==7) filterValue = asmFilter(actualDirection);
  this->eraseData();
}

void FilePGM::createMatrix (int w, int h, QFile *file) {
  intMatrix = 0;
  intMatrix = new int*[h];

  // Se crea un stream de lectura
  QByteArray fileContents;
  QTextStream *ioBuffer;

  // Cargamos la secuencia de datos
  fileContents = file->readAll().trimmed();
  ioBuffer = new QTextStream(fileContents);

  // Introducimos los datos en la matriz
  int i = h - 1;
  do {
    int j = w - 1;
    intMatrix[i] = new int[w];
    do {
      ioBuffer->operator >>(intMatrix[i][j]);
      j--;
    } while (j >= 0);
    i--;
  } while (i >= 0);
  ioBuffer->flush();
  ioBuffer->~QTextStream();
}

void FilePGM::calcCoocurrence (int x, int y, int w, int h) {
  int wSize = int(floor(windowSize / 2));
  int direction[4][2] = {{0,1},{-1,1},{-1,0},{-1,-1}};
  int ***matrix = 0;
  matrix = new int **[4];
  coocurrences = 0;
  coocurrences = new int **[4];

  int i = 3;
  do {
    selPixelsCount[i] = 0;
    i--;
  } while (i >= 0);

  i = 3;
  do {
    matrix[i] = new int *[intensity];
    coocurrences[i] = new int *[intensity];
    int j = intensity - 1;
    do {
      matrix[i][j] = new int[intensity];
      coocurrences[i][j] = new int[intensity];
      int k = intensity - 1;
      do {
        matrix[i][j][k] = 0;
        coocurrences[i][j][k] = 0;
        k--;
      } while (k >= 0);
      j--;
    } while (j >= 0);
    i--;
  } while (i >= 0);

  // Calculamos Matriz de Co-ocurrencia para las direcciones (0, 45, 90, 135)
  int near_ = 0;
  for (int dir = 0; dir < 4; dir++) {
    for (int i = (x + wSize); i < ((x + w) - (wSize)); i++) {
      for (int j = (y + wSize); j < ((y + h) - (wSize)); j++) {
        int actual = intMatrix[i][j];
        near_ = intMatrix[(i + (direction[dir][0] * wSize))][(j + (direction[dir][1] * wSize))];
        matrix[dir][actual][near_]++;
      }
    }
  }

  // Se traspone y se suma con las otras direcciones (180, 225, 270, 315)
  for (int dir = 0; dir < 4; dir++) {
    for (int i = 0; i < intensity; i++) {
      for (int j = 0; j < intensity; j++)
        coocurrences[dir][i][j] = matrix[dir][i][j] + matrix[dir][j][i];
      for (int j = 0; j < intensity; j++)
        selPixelsCount[dir] += coocurrences[dir][i][j];
    }
  }

  if (matrix != 0) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < intensity; j++)
        delete[] matrix[i][j];
      delete[] matrix[i];
    }
    delete[] matrix;
    matrix = 0;
  }
}

void FilePGM::calcNormalized () {
  normalized = 0;
  normalized = new double **[4];

  // Calculamos la normalizacion
  for (int i = 0; i < 4; i++) {
    normalized[i] = new double *[intensity];
    for (int j = 0; j < intensity; j++) {
      normalized[i][j] = new double[intensity];
      for (int k = 0; k < intensity; k++) {
        normalized[i][j][k] = double(double(coocurrences[i][j][k]) / selPixelsCount[i]);
      }
    }
  }
  if (this->coocurrences != 0) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < intensity; j++) {
        for (int k = 0; k < intensity; k++)
          this->coocurrences[i][j][k] = 0;
        delete[] this->coocurrences[i][j];
      }
      delete[] this->coocurrences[i];
    }
  }
  delete[] this->coocurrences;
  this->coocurrences = 0;
}

double FilePGM::homogenateFilter (int direction) {
  double homoIndex = 0;
  for (int i = 0; i < intensity; i++) {
    for (int j = 0; j < intensity; j++) {
      homoIndex += normalized[direction][i][j]/(1.0 + pow(((double)(i - j)), 2.0));
    }
  }
  return homoIndex;
}

double FilePGM::contrastFilter (int direction) {
  double contrastIndex = 0;
  for (int j = 0; j < intensity; j++) {
    for (int k = 0; k < intensity; k++) {
      contrastIndex = contrastIndex + (normalized[direction][j][k]*pow((double)(j - k), 2.0));
    }
  }
  return contrastIndex;
}

double FilePGM::dissimilarFilter (int direction) {
  double dissimilarIndex = 0;
  for (int j = 0; j < intensity; j++) {
    for (int k = 0; k < intensity; k++) {
      if ((j - k) >= 0) {
        dissimilarIndex = dissimilarIndex + (normalized[direction][j][k]*(j - k));
      } else {
        dissimilarIndex = dissimilarIndex + (normalized[direction][j][k]*(-1.0*(j - k)));
      }
    }
  }
  return dissimilarIndex;
}

double FilePGM::glcmFilter (int direction) {
  double glcmIndex = 0;
  for (int j = 0; j < intensity; j++) {
    for (int k = 0; k < intensity; k++) {
      glcmIndex = glcmIndex + normalized[direction][j][k] * j;
    }
  }
  return glcmIndex;
}

double FilePGM::entropyFilter (int direction) {
  double entropyIndex = 0;
  for (int j = 0; j < intensity; j++) {
    for (int k = 0; k < intensity; k++) {
      if (normalized[direction][j][k] != 0)
        entropyIndex -= (normalized[direction][j][k] * log(normalized[direction][j][k]));
    }
  }
  return entropyIndex;
}

double FilePGM::asmFilter (int direction) {
  double asmIndex = 0;
  for (int j = 0; j < intensity; j++) {
    for (int k = 0; k < intensity; k++) {
      asmIndex += (pow(normalized[direction][j][k], 2.0));
    }
  }
  return asmIndex;
}

double FilePGM::uniformityFilter (int direction) {
  return sqrt(asmFilter(direction));
}

double FilePGM::media (int direction, int index, bool invertIndex) {
  double miu = 0;
  if (invertIndex == false) {
    for (int i = 0; i < intensity; i++)
      miu += normalized[direction][index][i];
  } else {
    for (int i = 0; i < intensity; i++)
      miu += normalized[direction][i][index];
  }
  return (miu/intensity);
}

void FilePGM::eraseData() {
  if (this->coocurrences != 0) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < intensity; j++)
        delete[] this->coocurrences[i][j];
      delete[] this->coocurrences[i];
    }
    delete[] this->coocurrences;
    this->coocurrences = 0;
  }

  if (this->normalized != 0) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < intensity; j++)
        delete[] this->normalized[i][j];
      delete[] this->normalized[i];
    }
    delete[] this->normalized;
    this->normalized = 0;
  }
}

double FilePGM::standardDeviationFilter (int direction) {
  double standardDeviation = 0;
  for (int i = 0; i < intensity; i++) {
    for (int j = 0; j < intensity; j++) {
      standardDeviation += (normalized[direction][i][j]*pow((double)(i - media(direction, i, false)), 2.0));
    }
  }
  return sqrt(standardDeviation);
}

double FilePGM::correlationFilter(int direction) {
  double correlationIndex = 0;
  double standartDesviation = standardDeviationFilter(direction);
  for (int i = 0; i < intensity; i++) {
    for (int j = 0; j < intensity; j++) {
      correlationIndex += normalized[direction][i][j]*(((i-media(direction,i,false))*(j-media(direction,j,true)))/(sqrt(pow(standartDesviation,4.0))));
    }
  }
  return correlationIndex;
}

bool FilePGM::loadDCM (const string &nombreImagenDICOM, const string &nombreImagenPGM) {
  this->imageDICOM = 0;
  this->imageDICOM = new DicomImage(nombreImagenDICOM.c_str());
  //imageDCM->setMinMaxWindow();
  this->imageDICOM->setWindow(50.0, 150.0);//higado

  int grey, heightIN, widthIn, levelsIN;
  Uint8 *pixelData = (Uint8 *)(imageDICOM->getOutputData(8));
  if (pixelData == NULL)
  {
    cerr << "No se pudo obtener pixelData" << endl;
    delete this->imageDICOM;
    return false;
  }

  Uint8 *pix = pixelData;
  heightIN = widthIn=levelsIN=0;

  std::ofstream fileOut;
  fileOut.open(nombreImagenPGM.c_str(),ios::out);

  if (fileOut == 0)
  {
    cerr << " UnableToCreateFile " << nombreImagenPGM << endl;
    assert(false);
    return false;
  }

  widthIn = imageDICOM->getWidth();
  heightIN = imageDICOM->getHeight();
  levelsIN = 255;
  fileOut << "P2" << endl;
  fileOut << "#Auto-generated from " << QFileInfo(nombreImagenDICOM.c_str()).fileName().toLocal8Bit().data() << endl;
  fileOut << widthIn << " " << heightIN << endl;
  fileOut << levelsIN << endl;
  for(int filas = 0; filas < heightIN; filas++) {
    for(int cols = 0; cols < widthIn; cols++) {
      grey =* pix;
      fileOut << grey;
      fileOut << endl;
      pix++;
    }
  }
  fileOut.close();
  return true;
}

void FilePGM::cleanAll() {
  if (this->intMatrix != 0) {
    for (int i = 0; i < height; i++) {
      delete[] this->intMatrix[i];
    }
    delete[] this->intMatrix;
    this->intMatrix = 0;
  }
}

FilePGM::~FilePGM() {
  if (this->intMatrix != 0) {
    for (int i = 0; i < height; i++) {
      delete[] this->intMatrix[i];
    }
    delete[] this->intMatrix;
    this->intMatrix = 0;
  }
}
