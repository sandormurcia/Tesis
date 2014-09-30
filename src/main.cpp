#include <QApplication>
#include <QSplashScreen>
#include <QProgressBar>
#include <QDir>
#include <QFileInfoList>
#include <include/MainWindow.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;

void createCacheFile (Mat &m, const char* filename)
{
  FileStorage fs(filename, FileStorage::WRITE);
  fs << "Matrix" << m;
  fs.release();
}

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QPixmap pixmap(":/icons/splash.png");
  QSplashScreen *splash = new QSplashScreen(pixmap);
  QProgressBar *progress = new QProgressBar(splash);
  splash->setCursor(Qt::BusyCursor);
  progress->setGeometry(5, 285, 470, 30); // puts it at bottom
  progress->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  progress->setValue(0);
  progress->setEnabled(true);
  splash->show();
  splash->showMessage("Iniciando modulos");
  QDir repoDir("../repo");
  if (!repoDir.exists()) {
    repoDir.mkpath(".");
  }
  repoDir.setFilter(QDir::Files | QDir::NoSymLinks);
  repoDir.setSorting(QDir::Size | QDir::Reversed);
  QStringList repoFilter;
  repoFilter << "*.png";
  QDir cacheDir("../cache");
  if (!cacheDir.exists()) {
    cacheDir.mkpath(".");
  }
  QFileInfoList repository = repoDir.entryInfoList(repoFilter, QDir::Files, QDir::Type|QDir::Name);
  progress->setMaximum(repository.size());
  for (int i = 0; i < repository.size(); ++i) {
    QFileInfo repoInfo = repository.at(i);
    QFileInfo cacheInfo(repository.at(i).absoluteFilePath().replace("/repo/", "/cache/").replace(".png", ".yml"));
    if (!cacheInfo.exists()) {
      Mat m;
      m = imread(repository.at(i).absoluteFilePath().toUtf8().constData());
      createCacheFile(m, cacheInfo.absoluteFilePath().toUtf8().constData());
    }
    splash->showMessage(repoInfo.fileName(), Qt::AlignBottom|Qt::AlignHCenter, Qt::white);
    progress->setFormat("Cargando repositorio: " + repoInfo.fileName());
    progress->setValue(i);
    qApp->processEvents();
  }
  splash->close();
  MainWindow mainWindow;
  mainWindow.show();
  return app.exec();
}
