#include <QtGui>

#include "ImageViewer.h"

ImageViewer::ImageViewer()
{
  this->setStyleSheet ("* { font: 11px } QDockWidget::title { text-align: center; padding: 0; margin: 0; background: lightblue; font: bold; }");

  dragging = false;

  values = new int[9];
  //{3,2,1,4,8,0,5,6,7}
  values[0] = 3;
  values[1] = 2;
  values[2] = 1;
  values[3] = 4;
  values[4] = 8;
  values[5] = 0;
  values[6] = 5;
  values[7] = 6;
  values[8] = 7;

  imageLabel   = new QLabel;
  previewLabel = new QLabel;
  reposLabel = new QLabel;
  QLabel *nLabel = new QLabel();
  nLabel->setMinimumSize (205,1);

  imageLabel->setBackgroundRole(QPalette::Base);
  imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  imageLabel->setScaledContents(true);

  reposLabel->setBackgroundRole(QPalette::Base);
  reposLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  reposLabel->setScaledContents(false);

  reposArea = new QScrollArea;
  reposArea->setBackgroundRole(QPalette::Dark);
  reposArea->setMinimumWidth(600);
  reposArea->setWidget(reposLabel);
  reposArea->setVisible(false);

  repositoryViewer = new QDockWidget("Imagen seleccionada");
  repositoryViewer->setMaximumSize(600,600);
  repositoryViewer->setMinimumSize(600,600);
  repositoryViewer->move(QPoint((QApplication::desktop()->width()/2)-(repositoryViewer->width()/2),(QApplication::desktop()->height()/2)-(repositoryViewer->height()/2)));
  repositoryViewer->setWidget(reposArea);
  repositoryViewer->setFeatures(QDockWidget::NoDockWidgetFeatures);

  scrollArea = new QScrollArea;
  scrollArea->setBackgroundRole(QPalette::Dark);
  scrollArea->setMinimumWidth(600);
  scrollArea->setWidget(imageLabel);
  scrollArea->setVisible(false);

  previewLabel->setMaximumSize(200,200);
  previewLabel->setMinimumSize(200,200);
  previewLabel->setMargin(0);
  previewLabel->setScaledContents(true);

  rightPanel = new QDockWidget("Seleccion Actual");
  rightPanel->setTitleBarWidget(0);
  rightPanel->setWidget(previewLabel);
  rightPanel->setFeatures(QDockWidget::NoDockWidgetFeatures);

  QVBoxLayout *mainGrid = new QVBoxLayout();
  mainGrid->setAlignment (Qt::AlignCenter);

  optionDir = new QComboBox();
  optionWSize = new QComboBox();

  filtrosT << "Homogeneidad" << "Contraste" << "Disimilaridad" << "Media GLCM" << "Desviacion Estandar" << "Entropia" << "Correlacion" << "ASM";

  QStringList direccionesT;
  direccionesT << "0 grados" << "45 grados" << "90 grados" << "135 grados";

  QStringList wSizes;
  wSizes << "3x3" << "5x5" << "7x7" << "9x9" << "11x11" << "13x13" << "15x15" << "17x17" << "19x19" << "21x21";

  optionDir->addItems(direccionesT);
  optionWSize->addItems(wSizes);

  mainGrid->addWidget (optionDir);
  mainGrid->addWidget (optionWSize);
  for (int f = 0; f < filtrosT.size(); f++) {
    optionFilter.append(new QCheckBox(filtrosT.at(f)));
    mainGrid->addWidget (optionFilter.at(f));
  }

  filtros = new QGroupBox(tr("Direccion, Ventana y Filtro"));
  filtros->setLayout (mainGrid);

  filtersPanel = new QDockWidget("Filtros");
  filtersPanel->setFeatures(QDockWidget::NoDockWidgetFeatures);

  resultList = new QTreeWidget();
  QStringList labels;
  elements << "Ninguna" << "0%";
  labels << "Archivo" << "% Diferencia";
  resultList->setHeaderLabels(labels);
  resultList->setColumnCount(2);
  resultList->setAlternatingRowColors(true);
  resultList->setColumnWidth(0,116);
  resultList->setColumnWidth(1,65);
  resultList->setIndentation(5);

  items.append(new QTreeWidgetItem((QTreeWidget*)0, elements));
  resultList->insertTopLevelItems(0, items);

  resultPanel = new QDockWidget("Resultados");
  resultPanel->setMaximumWidth(200);
  resultPanel->setMinimumSize(200,186);
  resultPanel->setWidget(resultList);
  resultPanel->setFeatures(QDockWidget::NoDockWidgetFeatures);

  filtersPanel->setWidget(filtros);

  createActions();
  createMenus();
  createToolBars();
  createStatusBar();
  updateActions();

  setCentralWidget(scrollArea);
  setWindowTitle(tr("Visor de imagenes"));
  resize(1000, 600);
  setMinimumSize(1000, 620);
  setMaximumSize(1000, 620);
  setWindowIcon(QIcon(":/icons/window.png"));
  move(QPoint((QApplication::desktop()->width()/2)-(width()/2),(QApplication::desktop()->height()/2)-(height()/2)));
  setFocus();
  statusBar()->showMessage(tr("Seleccione una imagen para abrirla"));
  addDockWidget(Qt::RightDockWidgetArea,rightPanel,  Qt::Vertical);
  addDockWidget(Qt::RightDockWidgetArea,resultPanel, Qt::Vertical);

  rightPanel->close();
  filtersPanel->close();
  resultPanel->close();
  repositoryViewer->close();

  resultList->installEventFilter(this);
}

void ImageViewer::open()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Abrir archivo"), "./Imagenes", tr("Imagenes DICOM (*.dcm)"));
  if (!fileName.isEmpty())
  {
    QString tmpPGM = fileName;
    tmpPGM.replace(".dcm",".pgm",Qt::CaseInsensitive);

    if(pgm.loadDCM(fileName.toStdString(), tmpPGM.toStdString()))
    {

      if (!imageLoaded.load(tmpPGM))
      {
        QMessageBox::information(this, tr("Visor de Imagen"), tr("No se pudo cargar el archivo %1.").arg(fileName));
        return;
      }

      pgm.loadFile(tmpPGM.toLocal8Bit().data());
      pgm.initialize();//leo propiedades, creo la matriz donde guardo el archivo

      imageLabel->clear();
      setCursor(Qt::ArrowCursor);
      imageLabel->installEventFilter(this);

      imageLoaded = imageLoaded.convertToFormat(QImage::Format_RGB16, Qt::AutoColor);

      imageLabel->setPixmap(QPixmap::fromImage(imageLoaded));
      imageLabel->adjustSize();
      scrollArea->setVisible(true);

      previewLabel->setPixmap(QPixmap::fromImage(imageLoaded));

      updateActions();
      statusBar()->showMessage(tr("Imagen cargada con exito"));
    }
    else
    {
      QMessageBox::information(this, tr("Visor de Imagen"), tr("No se pudo cargar el archivo %1.").arg(fileName));
      return;
    }
  }
}

void ImageViewer::closeFile()
{
  imageLabel->setText(tr(""));
  scrollArea->setVisible(false);
  imageLabel->clear();
  imageLabel->adjustSize();
  updateActions();
  rightPanel->close();
  resultPanel->close();
  filtersPanel->close();
  filtros->close();
  startDrag = QPoint(0,0);
  endDrag = QPoint(0,0);
  //pgm.eraseData();
}

void ImageViewer::selectArea()
{
  cancArea->setEnabled(true);
}

void ImageViewer::cancelSelection()
{
  imageLabel->setPixmap(QPixmap::fromImage(imageLoaded));
  startDrag = QPoint(0,0);
  endDrag = QPoint(0,0);
  procArea->setEnabled(false);
  cancArea->setEnabled(false);
  previewLabel->clear();
  rightPanel->close();
  filtersPanel->close();
  resultPanel->close();

}

void ImageViewer::cancelSelection(bool option)
{
  option = option;
  QMessageBox msgBox;
  msgBox.setWindowTitle(tr("Cancelar seleccion"));
  msgBox.setText(tr("¿Desea cancelar su seleccion?"));
  msgBox.setIcon(QMessageBox::Question);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  switch (msgBox.exec()) {
  case QMessageBox::Yes:
    imageLabel->setPixmap(QPixmap::fromImage(imageLoaded));
    startDrag = QPoint(0,0);
    endDrag = QPoint(0,0);
    procArea->setEnabled(false);
    cancArea->setEnabled(false);
    previewLabel->clear();
    rightPanel->close();
    filtersPanel->close();
    filtros->hide();
    resultPanel->close();
    break;
  case QMessageBox::No:
    break;
  default:
    break;
  }
}

void ImageViewer::about()
{
  QMessageBox::about(this, tr("Acerca de"),tr(""));
}

bool ImageViewer::eventFilter(QObject *object, QEvent *event)
{
  // Eventos para el Contenedor de la Imagen
  if (object == imageLabel) {
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
    // Click Izquierdo
    if (event->type() == QEvent::MouseButtonPress && mouseEvent->button() == Qt::LeftButton) {
      startDrag = QPoint(0,0);
      endDrag   = QPoint(0,0);
      imageLabel->setPixmap(QPixmap::fromImage(imageLoaded));
      startDrag = mouseEvent->pos();
      dragging = true;
      return true;
    }
    // Soltar el Click Izquierdo
    if (event->type() == QEvent::MouseButtonRelease && mouseEvent->button() == Qt::LeftButton) {
      endDrag = mouseEvent->pos();

      int correctMinusA = 0, correctMinusB = 0;
      QPoint tmpAPoint, tmpBPoint;

      if (min(startDrag.x(),endDrag.x()) < 0) correctMinusA = abs(min(startDrag.x(),endDrag.x()));
      if (min(startDrag.y(),endDrag.y()) < 0) correctMinusB = abs(min(startDrag.y(),endDrag.y()));

      tmpAPoint.setX(min(startDrag.x(),endDrag.x())+correctMinusA);
      tmpAPoint.setY(min(startDrag.y(),endDrag.y())+correctMinusB);
      tmpBPoint.setX(max(startDrag.x(),endDrag.x())+correctMinusA);
      tmpBPoint.setY(max(startDrag.y(),endDrag.y())+correctMinusB);

      startDrag = tmpAPoint;
      endDrag   = tmpBPoint;

      procArea->setEnabled(true);

      rightPanel->show();
      filtersPanel->show();
      resultPanel->show();
      filtersPanel->setWidget(filtros);
      filtros->show();

      int allowedSizes[] = {50, 100, 150, 200, 300, 400, 500};

      int widthSel = endDrag.x() - startDrag.x();
      int heightSel = endDrag.y() - startDrag.y();
      int maxFromSel = max(widthSel, heightSel);

      maxFromSel = (maxFromSel > 500) ? 500 : maxFromSel;

      for (int i = 6; i >= 0; i--) {
        if ((allowedSizes[i] - max(widthSel, heightSel)) < 0) break;
        maxFromSel = allowedSizes[i];
      }

      if ((startDrag.x() + maxFromSel) > pgm.width ) startDrag.setX(pgm.width  - maxFromSel - 1);
      if ((startDrag.y() + maxFromSel) > pgm.height) startDrag.setY(pgm.height - maxFromSel - 1);

      endDrag = QPoint(startDrag.x()+maxFromSel,startDrag.y()+maxFromSel);

      QRectF selection(startDrag.x(),startDrag.y(),maxFromSel,maxFromSel);
      selPreview = imageLoaded.copy(startDrag.x(),
                                    startDrag.y(),
                                    int(selection.width()),
                                    int(selection.height())).scaled(200,200,Qt::KeepAspectRatio,Qt::SmoothTransformation);

      QPixmap preview = QPixmap::fromImage(selPreview);
      QPixmap tmpPix(QPixmap::fromImage(imageLoaded));

      QPainter painter(&tmpPix);
      painter.setPen(QPen(QColor(106,230,147), 1, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin));
      painter.setBrush(QBrush(QColor(106,230,147)));
      painter.setOpacity(0.3);
      painter.drawRect(selection);
      imageLabel->setPixmap(tmpPix);
      painter.end();

      painter.begin(&preview);
      painter.setPen(QPen(QColor(255,0,0)));
      painter.setFont(QFont("",7));
      painter.drawText(2,10,QString::number(maxFromSel)+" x "+QString::number(maxFromSel));
      painter.drawImage(QPoint(1,13),QImage(":/icons/dir"+QString::number(optionDir->currentIndex())+".png"));
      painter.drawText(20,20,optionWSize->currentText());
      painter.end();

      previewLabel->setPixmap(preview);

      pgm.makeSelection(startDrag.x(), startDrag.y(), endDrag.x(), endDrag.y());
      statusBar()->showMessage("Seleccionado: "+QString::number(maxFromSel)+" x "+QString::number(maxFromSel));
      dragging = false;

      return true;
    }
    // Mover el mouse
    if (event->type() == QEvent::MouseMove && mouseEvent->buttons() == Qt::LeftButton) {
      dragging = true;

      QRectF selection(startDrag, mouseEvent->pos());
      QPixmap tmpPix(QPixmap::fromImage(imageLoaded));

      QPainter painter(&tmpPix);
      painter.setPen(QPen(QColor(106,230,147), 1, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin));
      painter.setBrush(QBrush(QColor(106,230,147)));
      painter.setOpacity(0.3);
      painter.drawRect(selection);
      imageLabel->setPixmap(tmpPix);
      painter.end();

      tmpPix = QPixmap(*imageLabel->pixmap());

      painter.begin(&tmpPix);

      painter.setPen(QPen(QColor(255,0,0), 1, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin));
      painter.drawLine(mouseEvent->pos().x(),0,mouseEvent->pos().x(),pgm.width);
      painter.drawLine(0,mouseEvent->pos().y(),pgm.height,mouseEvent->pos().y());
      painter.setPen(QPen(QColor(0,0,255), 1, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin));
      painter.drawLine(startDrag.x(),0,startDrag.x(),pgm.width);
      painter.drawLine(0,startDrag.y(),pgm.height,startDrag.y());
      imageLabel->setPixmap(tmpPix);

      restrictPoints();
      selectArea();

      statusBar()->showMessage("Seleccion: "+QString::number(abs(startDrag.x() - mouseEvent->pos().x()))+" x "+QString::number(abs(startDrag.y() - mouseEvent->pos().y())));
      return true;
    } else if (event->type() == QEvent::HoverMove) {
      if (!dragging) {

        QPixmap tmpPix(QPixmap::fromImage(imageLoaded));
        QRectF selection(startDrag, endDrag);

        QPainter painter(&tmpPix);
        painter.setPen(QPen(QColor(106,230,147), 1, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin));
        painter.setBrush(QBrush(QColor(106,230,147)));
        painter.setOpacity(0.3);
        painter.drawRect(selection);
        imageLabel->setPixmap(tmpPix);
        painter.end();

        tmpPix = QPixmap(*imageLabel->pixmap());

        painter.begin(&tmpPix);

        painter.setPen(QPen(QColor(255,0,0), 1, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin));
        painter.setBrush(QBrush(QColor(255,0,0)));
        painter.drawLine(0,mouseEvent->pos().x(),pgm.width,mouseEvent->pos().x());
        painter.drawLine(mouseEvent->pos().y(),0,mouseEvent->pos().y(),pgm.height);
        imageLabel->setPixmap(tmpPix);

        statusBar()->showMessage(QString::number(mouseEvent->pos().x())+" , "+QString::number(mouseEvent->pos().y()));
      }
      return true;
    }
  }
  if (object == reposLabel) {
  }
  return false;
}

void ImageViewer::createActions()
{
  openAct = new QAction(QIcon(":/icons/open.png"), tr("&Abrir..."), this);
  openAct->setShortcut(tr("Ctrl+O"));
  openAct->setStatusTip(tr("Abrir archivo"));
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

  closeAct = new QAction(QIcon(":/icons/close.png"), tr("&Cerrar..."), this);
  closeAct->setShortcut(tr("Ctrl+W"));
  closeAct->setStatusTip(tr("Cerrar archivo"));
  connect(closeAct, SIGNAL(triggered()), this, SLOT(closeFile()));

  exitAct = new QAction(tr("&Salir"), this);
  exitAct->setShortcut(tr("Alt+F4"));
  exitAct->setStatusTip(tr("Salir de la aplicacion"));
  connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

  procArea = new QAction(QIcon(":/icons/selectArea.png"), tr("&Procesar area..."), this);
  procArea->setStatusTip(tr("Procesar Area"));
  connect(procArea, SIGNAL(triggered()), this, SLOT(processArea()));

  cancArea = new QAction(QIcon(":/icons/cancelArea.png"), tr("&Cancelar seleccion..."), this);
  cancArea->setStatusTip(tr("Cancelar seleccion"));
  connect(cancArea, SIGNAL(triggered()), this, SLOT(cancelSelection()));
  cancArea->setEnabled(false);

  aboutAct = new QAction(tr("&Acerca de"), this);
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

  connect(resultList, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(openRepositoryImage(QTreeWidgetItem*,int)));

  connect(optionDir, SIGNAL(currentIndexChanged(int)), this, SLOT(changeFilterValues(int)));
  connect(optionWSize, SIGNAL(currentIndexChanged(int)), this, SLOT(changeFilterValues(int)));
}

void ImageViewer::changeFilterValues (int index) {
  previewLabel->clear();
  QRectF selection(startDrag, endDrag);
  int maxFromSel = endDrag.x() - startDrag.x();

  QPixmap preview(QPixmap::fromImage(selPreview));

  QPainter painter(&preview);
  painter.setPen(QPen(QColor(255,0,0)));
  painter.setFont(QFont("",7));
  painter.drawText(2,10,QString::number(maxFromSel)+" x "+QString::number(maxFromSel));
  painter.drawImage(QPoint(1,13),QImage(":/icons/dir"+QString::number(optionDir->currentIndex())+".png"));
  painter.drawText(20,20,optionWSize->currentText());
  painter.end();

  previewLabel->setPixmap(preview);
}

void ImageViewer::openRepositoryImage(QTreeWidgetItem *item, int column)
{
  reposLabel->clear();
  QImage tmpPreview = QImage(QString("./Repositorio/").append(item->text(0)));
  setCursor(Qt::ArrowCursor);
  reposLabel->installEventFilter(this);

  QPixmap tmpPix(QPixmap::fromImage(tmpPreview));
  QRectF selection(item->text(2).toInt(), item->text(3).toInt(), pgm.selWidth, pgm.selHeight);

  QPainter painter(&tmpPix);
  painter.setPen(QPen(QColor(106,230,147), 1, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin));
  painter.setOpacity(0.5);
  painter.drawRect(selection);
  reposLabel->setPixmap(tmpPix);
  painter.end();

  reposLabel->adjustSize();

  repositoryViewer->show();
  repositoryViewer->setWindowTitle(item->text(0).append(" con ").append(item->text(1).append("% de Diferencia de la muestra")));
  qDebug() << item->text(2) << item->text(3) << item->text(4) << item->text(5);
  repositoryViewer->setFocus();
}

void ImageViewer::createMenus()
{
  fileMenu = new QMenu(tr("&Archivo"), this);
  fileMenu->addAction(openAct);
  fileMenu->addAction(closeAct);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);

  viewMenu = new QMenu(tr("A&cciones"), this);
  viewMenu->addAction(procArea);
  viewMenu->addAction(cancArea);

  helpMenu = new QMenu(tr("&Ayuda"), this);
  helpMenu->addAction(aboutAct);

  menuBar()->addMenu(fileMenu);
  menuBar()->addMenu(viewMenu);
  menuBar()->addMenu(helpMenu);
}

void ImageViewer::createToolBars()
{
  mainToolBar = addToolBar(tr(""));
  mainToolBar->addAction(openAct);
  mainToolBar->addAction(closeAct);
  mainToolBar->addSeparator();
  mainToolBar->addAction(procArea);
  mainToolBar->addAction(cancArea);
  mainToolBar->setIconSize(QSize(20,20));
  mainToolBar->setMovable(false);
}

void ImageViewer::createStatusBar()
{
  statusBar()->showMessage(tr("Listo"));
}

void ImageViewer::updateActions()
{
  if(scrollArea->isVisible()) {
    closeAct->setEnabled(true);
    procArea->setEnabled(false);
  } else {
    closeAct->setEnabled(false);
    procArea->setEnabled(false);
  }
}

void ImageViewer::restrictPoints()
{
  if (endDrag.x() < 0) endDrag = QPoint(0,endDrag.y());
  if (endDrag.y() < 0) endDrag = QPoint(endDrag.x(),0);
  if (endDrag.x() > imageLoaded.width()) endDrag = QPoint(imageLoaded.width(),endDrag.y());
  if (endDrag.y() > imageLoaded.height()) endDrag = QPoint(endDrag.x(), imageLoaded.height());
}

void ImageViewer::processArea()
{
  cancArea->setEnabled(false);
  int minimum = min(endDrag.x()-startDrag.x(),(endDrag.y()-startDrag.y()));
  int maxWindowSize = 0;
  if (minimum % 2 == 0)
    maxWindowSize = minimum - 1;
  else
    maxWindowSize = minimum;

  if ((((endDrag.x()-startDrag.x()) * (endDrag.y()-startDrag.y())) < 400) || maxWindowSize < 3) {
    QMessageBox::warning(this, tr("No se puede procesar el area de seleccion"),tr("El area de seleccion actual es demasiado pequena para ser procesada.\nIntente con una un poco mas grande."),QMessageBox::Ok);
    return;
  }
  pgm.actualDirection = optionDir->currentIndex();
  pgm.windowSize = (2 * optionWSize->currentIndex()) + 3;

  double epsilon = 0.0;
  int checked = 0;

  if (pgm.windowSize<=maxWindowSize) {
    for (int f = 0; f < filtrosT.size(); f++)
      if (optionFilter.at(f)->isChecked() == true) checked++;

    for (int f = 0; f < filtrosT.size(); f++) {
      if (optionFilter.at(f)->isChecked()) {
        pgm.actualFilter = f;
        pgm.applyFilter();
        if (checked > 1)
          epsilon += pow(pgm.filterValue,2.0);
        else if (checked > 0)
          epsilon += pgm.filterValue;
      }
    }
    if (checked > 1)
      pgm.filterValue = sqrt(epsilon);
    else if (checked > 0)
      pgm.filterValue = epsilon;

    this->searchInRepository();
  } else
    QMessageBox::warning(this, tr("No se puede procesar el area de seleccion"),tr("Las dimensiones de la ventana sobrepasan el limite del area de seleccion.\nIntente con una ventana inferior o igual a ").append(QString::number(maxWindowSize)).append("x").append(QString::number(maxWindowSize)),QMessageBox::Ok);
  return;
}

void ImageViewer::searchInRepository()
{
  QDir *dir = new QDir("./Repositorio");

  dir->setFilter(QDir::Files | QDir::NoSymLinks);
  dir->setSorting(QDir::Size | QDir::Reversed);

  QStringList *nameFilter = new QStringList;
  nameFilter->operator <<("*.pgm");

  QFileInfoList list = dir->entryInfoList(*nameFilter, QDir::Files, QDir::Type|QDir::Name);
  double *absDiff = new double[list.size()];
  QStringList *coordinates = new QStringList;
  QStringList *result = new QStringList;
  items.clear();

  for (int x = 0; x < list.size(); ++x)
    absDiff[x] = 999999;

  int checked = 0;
  for (int f = 0; f < filtrosT.size(); f++)
    if (optionFilter.at(f)->isChecked() == true) checked++;

  for (int image = 0; image < list.size()/*3*/; ++image) {
    QFileInfo *fileInfo = new QFileInfo(list.at(image));
    QString fileName = fileInfo->absolutePath().append("/").append(fileInfo->fileName());
    result->clear();

    file_.loadFile(fileName);
    file_.initialize();
    int i = (file_.width - pgm.selWidth) - 1;
    do {
      int j = (file_.height - pgm.selHeight) - 1;
      do {
        double epsilon = 0.0;
        file_.actualDirection = optionDir->currentIndex();
        file_.windowSize = (2 * optionWSize->currentIndex()) + 3;
        file_.makeSelection(i,j,(i + pgm.selWidth),(j + pgm.selHeight));

        // Reemplazo file_.applyFilter();
        for (int f = 0; f < filtrosT.size(); f++) {
          if (optionFilter.at(f)->isChecked() == true) {
            file_.actualFilter = f;
            file_.applyFilter();
            if (checked > 1)
              epsilon += pow(file_.filterValue,2.0);
            else if (checked > 0)
              epsilon += file_.filterValue;
          }
        }
        if (checked > 1)
          file_.filterValue = sqrt(epsilon);
        else if (checked > 0)
          file_.filterValue = epsilon;

        if (absDiff[image] > fabs(pgm.filterValue - file_.filterValue)) {
          absDiff[image] = fabs(pgm.filterValue - file_.filterValue);
          coordinates->clear();
          coordinates->operator <<(QString::number(i));
          coordinates->operator <<(QString::number(j));
          coordinates->operator <<(QString::number(i + pgm.selWidth));
          coordinates->operator <<(QString::number(j + pgm.selHeight));
        }
        file_.eraseData();
        j--;
      } while (j >= 0);
      i--;
    } while (i >= 0);
    double percent = double(round(double(absDiff[image] * 100 / pgm.filterValue) * 100) / 100);
    QBrush bgcolor = QBrush(Qt::green);
    QString per_ = QString::number(percent);
    if (percent > 2.0) bgcolor = QBrush(Qt::red);
    result->operator <<(fileInfo->fileName());
    result->operator <<(per_);
    result->operator <<(coordinates->at(0));
    result->operator <<(coordinates->at(1));
    result->operator <<(coordinates->at(2));
    result->operator <<(coordinates->at(3));
    QTreeWidgetItem *item = new QTreeWidgetItem(*result);
    item->setForeground(1,bgcolor);
    items.append(item);
    qDebug() << fileInfo->fileName().toLocal8Bit().data() << pgm.filterValue << file_.filterValue << absDiff[image];
    //file_.FilePGM::~FilePGM();
  }
  file_.cleanAll();

  resultList->clear();
  resultList->insertTopLevelItems(0, items);
  resultList->sortByColumn(1,Qt::AscendingOrder);

  resultPanel->setWindowTitle(QString("Filtro Referencia: "+QString::number(pgm.filterValue)).toLocal8Bit().data());
}
