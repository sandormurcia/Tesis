#include <QApplication>
#include <QSplashScreen>
#include "ImageViewer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QPixmap pixmap(":/icons/Splash.png");
    QSplashScreen *splash = new QSplashScreen(pixmap);
    splash->show();
    splash->showMessage("Iniciando modulos");

    QDir dir("./Repositorio");

    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setSorting(QDir::Size | QDir::Reversed);

    QStringList nameFilter;
    nameFilter << "*.dcm";

    FilePGM file_;

    QFileInfoList list  = dir.entryInfoList(nameFilter, QDir::Files, QDir::Type|QDir::Name);

    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);

        QString fileName = fileInfo.absolutePath().append("/").append(fileInfo.fileName());
        QString tmpPGM   = fileInfo.absolutePath().append("/").append(fileInfo.baseName().append(".pgm"));
        QFileInfo search(tmpPGM.toLocal8Bit().data());

        if (search.exists() == false) {
            splash->clearMessage();
            if(file_.loadDCM(fileName.toStdString(),tmpPGM.toStdString())) {
                qDebug() << "Generando " << fileInfo.baseName().append(".pgm") << " de " << fileInfo.fileName();
                splash->showMessage("Generando Repositorio\nGenerado "+fileInfo.fileName(),Qt::AlignBottom|Qt::AlignHCenter,Qt::white);
            } else {
                splash->showMessage("Generando Repositorio\nError en "+fileInfo.fileName(),Qt::AlignBottom|Qt::AlignHCenter,Qt::white);
            }
        }

        qApp->processEvents();
    }

    splash->close();

    ImageViewer imageViewer;
    imageViewer.show();

    return app.exec();
}
