#include <include/MainWindow.h>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
  this->setStyleSheet ("* { font: 11px } QDockWidget::title { text-align: center; padding: 0; margin: 0; background: lightblue; font: bold; }");

  dragging = false;
  // {3,2,1,4,8,0,5,6,7}
  this->values[0] = 3;
  this->values[1] = 2;
  this->values[2] = 1;
  this->values[3] = 4;
  this->values[4] = 8;
  this->values[5] = 0;
  this->values[6] = 5;
  this->values[7] = 6;
  this->values[8] = 7;

  referenceLabel   = new QLabel;
  repositoryLabel = new QLabel;
  QLabel *nLabel = new QLabel();
  nLabel->setMinimumSize (205,1);

  referenceLabel->setBackgroundRole(QPalette::Base);
  referenceLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  referenceLabel->setScaledContents(true);

  repositoryLabel->setBackgroundRole(QPalette::Base);
  repositoryLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  repositoryLabel->setScaledContents(false);

  reposArea = new QScrollArea;
  reposArea->setBackgroundRole(QPalette::Dark);
  reposArea->setMinimumWidth(600);
  reposArea->setWidget(repositoryLabel);
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
  scrollArea->setWidget(referenceLabel);
  scrollArea->setVisible(false);

  QVBoxLayout *mainGrid = new QVBoxLayout();
  mainGrid->setAlignment (Qt::AlignCenter);

  optionDir = new QComboBox();
  optionWSize = new QComboBox();
  optionPathType = new QComboBox();

  filtrosT << "Homogeneidad" << "Contraste" << "Disimilaridad" << "Media GLCM" << "Desviacion Estandar" << "Entropia" << "Correlacion" << "ASM" << "Uniformidad";

  QStringList direccionesT;
  direccionesT << "0 grados" << "45 grados" << "90 grados" << "135 grados";

  QStringList wSizes;
  wSizes << "3x3" << "5x5" << "7x7" << "9x9" << "11x11" << "13x13" << "15x15" << "17x17" << "19x19" << "21x21";

  QStringList wPaths;
  wPaths << "Única caracterización" << "Conjunto de caracteristicas";

  optionDir->addItems(direccionesT);
  optionWSize->addItems(wSizes);
  optionPathType->addItems(wPaths);

  mainGrid->addWidget (optionDir);
  mainGrid->addWidget (optionWSize);
  mainGrid->addWidget (optionPathType);
  for (int f = 0; f < filtrosT.size(); f++) {
    optionFilter.append(new QCheckBox(filtrosT.at(f)));
    mainGrid->addWidget (optionFilter.at(f));
  }

  filtersGroup = new QGroupBox();
  filtersGroup->setLayout (mainGrid);

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

  filtersPanel->setWidget(filtersGroup);

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
  setDockNestingEnabled(true);
  setDockOptions(QMainWindow::AllowNestedDocks|QMainWindow::AllowTabbedDocks);
  addDockWidget(Qt::RightDockWidgetArea, filtersPanel, Qt::Vertical);
  addDockWidget(Qt::RightDockWidgetArea, resultPanel, Qt::Vertical);
  tabifyDockWidget(filtersPanel, resultPanel);
  filtersPanel->close();
  resultPanel->close();
  repositoryViewer->close();

  resultList->installEventFilter(this);
}

void MainWindow::open()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Abrir archivo"), "../", tr("Imagenes medicas (*.png *.pgm)"));
  if (!fileName.isEmpty() && imageLoaded.load(fileName)) {
    string filePath = fileName.toUtf8().constData();
    this->referenceImage = new PNGFile(filePath);
    referenceLabel->clear();
    setCursor(Qt::ArrowCursor);
    referenceLabel->installEventFilter(this);
    imageLoaded = imageLoaded.convertToFormat(QImage::Format_RGB16, Qt::AutoColor);
    referenceLabel->setPixmap(QPixmap::fromImage(imageLoaded));
    referenceLabel->adjustSize();
    scrollArea->setVisible(true);
    updateActions();
    statusBar()->showMessage(tr("Imagen cargada con exito"));
  } else {
    if (!fileName.isEmpty())
      QMessageBox::information(this, tr("Visor de Imagen"), tr("No se pudo cargar el archivo %1.").arg(fileName));
    return;
  }
}

void MainWindow::closeFile()
{
  referenceLabel->setText(tr(""));
  scrollArea->setVisible(false);
  referenceLabel->clear();
  referenceLabel->adjustSize();
  updateActions();
  resultPanel->close();
  filtersPanel->close();
  filtersGroup->close();
  startDrag = QPoint(0,0);
  endDrag = QPoint(0,0);
}

void MainWindow::selectArea()
{
  cancArea->setEnabled(true);
}

void MainWindow::cancelSelection()
{
  referenceLabel->setPixmap(QPixmap::fromImage(imageLoaded));
  startDrag = QPoint(0,0);
  endDrag = QPoint(0,0);
  procArea->setEnabled(false);
  cancArea->setEnabled(false);
  filtersPanel->close();
  resultPanel->close();

}

void MainWindow::cancelSelection(bool option)
{
  option = option;
  QMessageBox msgBox;
  msgBox.setWindowTitle(tr("Cancelar seleccion"));
  msgBox.setText(tr("¿Desea cancelar su seleccion?"));
  msgBox.setIcon(QMessageBox::Question);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  switch (msgBox.exec()) {
    case QMessageBox::Yes:
      referenceLabel->setPixmap(QPixmap::fromImage(imageLoaded));
      startDrag = QPoint(0,0);
      endDrag = QPoint(0,0);
      procArea->setEnabled(false);
      cancArea->setEnabled(false);
      filtersPanel->close();
      filtersGroup->hide();
      resultPanel->close();
      break;
    case QMessageBox::No:
      break;
    default:
      break;
  }
}

void MainWindow::about()
{
  QMessageBox::about(this, tr("Acerca de"),tr(""));
}

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
  // Eventos para el Contenedor de la Imagen
  if (object == referenceLabel) {
    QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
    // Click Izquierdo
    if (event->type() == QEvent::MouseButtonPress && mouseEvent->button() == Qt::LeftButton) {
      startDrag = QPoint(0,0);
      endDrag   = QPoint(0,0);
      referenceLabel->setPixmap(QPixmap::fromImage(imageLoaded));
      startDrag = mouseEvent->pos();
      dragging = true;
      return true;
    }
    // Soltar el Click Izquierdo
    if (event->type() == QEvent::MouseButtonRelease && mouseEvent->button() == Qt::LeftButton) {
      endDrag = mouseEvent->pos();

      int correctMinusA = 0, correctMinusB = 0;
      QPoint tmpAPoint, tmpBPoint;

      if (qMin(startDrag.x(),endDrag.x()) < 0) correctMinusA = qAbs(qMin(startDrag.x(),endDrag.x()));
      if (qMin(startDrag.y(),endDrag.y()) < 0) correctMinusB = qAbs(qMin(startDrag.y(),endDrag.y()));

      tmpAPoint.setX(qMin(startDrag.x(),endDrag.x())+correctMinusA);
      tmpAPoint.setY(qMin(startDrag.y(),endDrag.y())+correctMinusB);
      tmpBPoint.setX(qMax(startDrag.x(),endDrag.x())+correctMinusA);
      tmpBPoint.setY(qMax(startDrag.y(),endDrag.y())+correctMinusB);

      startDrag = tmpAPoint;
      endDrag   = tmpBPoint;

      procArea->setEnabled(true);

      filtersPanel->show();
      resultPanel->show();
      filtersPanel->setWidget(filtersGroup);
      filtersGroup->show();

      int allowedSizes[] = {12, 50, 100, 150, 200, 300, 400, 500};

      int widthSel = endDrag.x() - startDrag.x();
      int heightSel = endDrag.y() - startDrag.y();
      int maxFromSel = qMax(widthSel, heightSel);

      maxFromSel = (maxFromSel > 500) ? 500 : maxFromSel;

      for (int i = 7; i >= 0; i--) {
        if ((allowedSizes[i] - qMax(widthSel, heightSel)) < 0) break;
        maxFromSel = allowedSizes[i];
      }

      if ((startDrag.x() + maxFromSel) > imageLoaded.width()) startDrag.setX(imageLoaded.width() - maxFromSel - 1);
      if ((startDrag.y() + maxFromSel) > imageLoaded.height()) startDrag.setY(imageLoaded.height() - maxFromSel - 1);

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
      referenceLabel->setPixmap(tmpPix);
      painter.end();

      painter.begin(&preview);
      painter.setPen(QPen(QColor(255,0,0)));
      painter.setFont(QFont("",7));
      painter.drawText(2,10,QString::number(maxFromSel)+" x "+QString::number(maxFromSel));
      painter.drawImage(QPoint(1,13),QImage(":/icons/dir"+QString::number(optionDir->currentIndex())+".png"));
      painter.drawText(20,20,optionWSize->currentText());
      painter.end();

      this->referenceImage->makeSelection(startDrag.x(), startDrag.y(), endDrag.x(), endDrag.y());
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
      referenceLabel->setPixmap(tmpPix);
      painter.end();

      tmpPix = QPixmap(*referenceLabel->pixmap());

      painter.begin(&tmpPix);

      painter.setPen(QPen(QColor(255,0,0), 1, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin));
      painter.drawLine(mouseEvent->pos().x(), 0, mouseEvent->pos().x(), imageLoaded.height());
      painter.drawLine(0,mouseEvent->pos().y(),imageLoaded.width(),mouseEvent->pos().y());
      painter.setPen(QPen(QColor(0,0,255), 1, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin));
      painter.drawLine(startDrag.x(), 0, startDrag.x(), imageLoaded.height());
      painter.drawLine(0, startDrag.y(), imageLoaded.width(), startDrag.y());
      referenceLabel->setPixmap(tmpPix);

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
        referenceLabel->setPixmap(tmpPix);
        painter.end();

        tmpPix = QPixmap(*referenceLabel->pixmap());

        painter.begin(&tmpPix);

        painter.setPen(QPen(QColor(255,0,0), 1, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin));
        painter.setBrush(QBrush(QColor(255,0,0)));
        painter.drawLine(0, mouseEvent->pos().x(), imageLoaded.width(), mouseEvent->pos().x());
        painter.drawLine(mouseEvent->pos().y(), 0, mouseEvent->pos().y(),imageLoaded.height());
        referenceLabel->setPixmap(tmpPix);

        statusBar()->showMessage(QString::number(mouseEvent->pos().x())+" , "+QString::number(mouseEvent->pos().y()));
      }
      return true;
    }
  }
  if (object == repositoryLabel) {
  }
  return false;
}

void MainWindow::createActions()
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

  connect(optionDir, SIGNAL(currentIndexChanged(int)), this, SLOT(changeFilterValuesWithRecalculation(int)));
  connect(optionWSize, SIGNAL(currentIndexChanged(int)), this, SLOT(changeFilterValuesWithRecalculation(int)));
  connect(optionPathType, SIGNAL(currentIndexChanged(int)), this, SLOT(changeFilterValues(int)));
}

void MainWindow::changeFilterValues (int index) {
}

void MainWindow::changeFilterValuesWithRecalculation (int index) {
  this->referenceImage->mayRecalculate = true;
}

void MainWindow::openRepositoryImage(QTreeWidgetItem *item, int column)
{
  this->repositoryLabel->clear();
  QImage tmpPreview = QImage(QString("../repo/").append(item->text(0)));
  setCursor(Qt::ArrowCursor);
  this->repositoryLabel->installEventFilter(this);

  QPixmap tmpPix(QPixmap::fromImage(tmpPreview));
  QRectF selection(item->text(2).toInt(), item->text(3).toInt(), this->referenceImage->selWidth, this->referenceImage->selHeight);

  QPainter painter(&tmpPix);
  painter.setPen(QPen(QColor(106,230,147), 1, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin));
  painter.setOpacity(0.5);
  painter.drawRect(selection);
  this->repositoryLabel->setPixmap(tmpPix);
  painter.end();

  this->repositoryLabel->adjustSize();

  this->repositoryViewer->show();
  this->repositoryViewer->setWindowTitle(item->text(0).append(" con ").append(item->text(1).append("% de Diferencia de la muestra")));
  this->repositoryViewer->setFocus();
}

void MainWindow::createMenus()
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

void MainWindow::createToolBars()
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

void MainWindow::createStatusBar()
{
  statusBar()->showMessage(tr("Listo"));
}

void MainWindow::updateActions()
{
  if(scrollArea->isVisible()) {
    closeAct->setEnabled(true);
    procArea->setEnabled(false);
  } else {
    closeAct->setEnabled(false);
    procArea->setEnabled(false);
  }
}

void MainWindow::restrictPoints()
{
  if (endDrag.x() < 0) endDrag = QPoint(0, endDrag.y());
  if (endDrag.y() < 0) endDrag = QPoint(endDrag.x(), 0);
  if (endDrag.x() > imageLoaded.width()) endDrag = QPoint(imageLoaded.width(),endDrag.y());
  if (endDrag.y() > imageLoaded.height()) endDrag = QPoint(endDrag.x(), imageLoaded.height());
  if (startDrag.x() < 0) startDrag = QPoint(0, startDrag.y());
  if (startDrag.y() < 0) startDrag = QPoint(startDrag.x(), 0);
}

void MainWindow::processArea()
{
  cancArea->setEnabled(false);
  int actualBadgeSize = optionPathType->currentIndex() == 0 ? 1 : 4;
  if (actualBadgeSize != this->referenceImage->badgeSize || this->referenceImage->started != true) {
    this->referenceImage->badgeSize = actualBadgeSize;
    this->referenceImage->startDataPlaceholders ();
  }
  int minimum = qMin(endDrag.x()-startDrag.x(),(endDrag.y()-startDrag.y()));
  int maxWindowSize = 0;
  if (minimum % 2 == 0)
    maxWindowSize = minimum - 1;
  else
    maxWindowSize = minimum;

  if ((((endDrag.x()-startDrag.x()) * (endDrag.y()-startDrag.y())) < 144) || maxWindowSize < 3) {
    QMessageBox::warning(this, tr("No se puede procesar el area de seleccion"),tr("El area de seleccion actual es demasiado pequena para ser procesada.\nIntente con una un poco mas grande."),QMessageBox::Ok);
    return;
  }
  this->referenceImage->actualDirection = optionDir->currentIndex();
  this->referenceImage->windowSize = (2 * optionWSize->currentIndex()) + 3;

  double epsilon[filtrosT.size()];
  for (int f = 0; f < filtrosT.size(); f++)
    epsilon[f] = 0;

  int checked = 0;
  if (this->referenceImage->windowSize<=maxWindowSize) {
    for (int f = 0; f < filtrosT.size(); f++)
      if (optionFilter.at(f)->isChecked() == true) checked++;

    for(int b = 0; b < this->referenceImage->badgeSize; b++) {
      for (int f = 0; f < filtrosT.size(); f++) {
        if (optionFilter.at(f)->isChecked()) {
          this->referenceImage->applyFilter(f);
          epsilon[f] += (this->referenceImage->filterValue[b][f] / this->referenceImage->badgeSize);
        }
      }
    }
    qDebug() << "Valores para imágen de referencia:";
    for (int f = 0; f < filtrosT.size(); f++) {
      if (optionFilter.at(f)->isChecked()) {
        qDebug() << filtrosT.at(f) << ":" << epsilon[f];
      }
    }
    this->searchInRepository(epsilon);
  } else
    QMessageBox::warning(this, tr("No se puede procesar el area de seleccion"),tr("Las dimensiones de la ventana sobrepasan el limite del area de seleccion.\nIntente con una ventana inferior o igual a ").append(QString::number(maxWindowSize)).append("x").append(QString::number(maxWindowSize)),QMessageBox::Ok);
  return;
}

void MainWindow::searchInRepository(double *referenceValues)
{
  QDir *dir = new QDir("../repo");

  dir->setFilter(QDir::Files | QDir::NoSymLinks);
  dir->setSorting(QDir::Size | QDir::Reversed);

  QStringList *nameFilter = new QStringList;
  nameFilter->operator <<("*.png");

  QFileInfoList list = dir->entryInfoList(*nameFilter, QDir::Files, QDir::Type|QDir::Name);
  double absoluteSimilarity[list.size()];
  QStringList *coordinates = new QStringList;
  QStringList *result = new QStringList;
  items.clear();

  int checked = 0;
  double epsilon[filtrosT.size()], percent = 0;
  for (int f = 0; f < filtrosT.size(); f++)
    if (optionFilter.at(f)->isChecked() == true) checked++;

  double absoluteReference = 0;
  for (int f = 0; f < filtrosT.size(); f++) {
    if (optionFilter.at(f)->isChecked()) {
      absoluteReference += (referenceValues[f] / checked);
    }
  }

  PNGFile *repositoryImage;
  for (int image = 0; image < list.size(); ++image) {
    bool found = false;
    QFileInfo *fileInfo = new QFileInfo(list.at(image));
    QString fileName = fileInfo->absolutePath().append("/").append(fileInfo->fileName());
    qDebug() << "Cargando archivo" << fileName;
    result->clear();
    QTime time;
    time.start();
    string filePath = fileName.toUtf8().constData();
    repositoryImage = new PNGFile(filePath);
    repositoryImage->badgeSize = this->referenceImage->badgeSize;
    repositoryImage->startDataPlaceholders ();
    double progress = 0.0;
    int i = (repositoryImage->height - this->referenceImage->selHeight) - 1;
    absoluteSimilarity[image] = 0;
    do {
      int j = (repositoryImage->width - this->referenceImage->selWidth) - 1;
      do {
        for (int f = 0; f < filtrosT.size(); f++)
          epsilon[f] = 0;

        repositoryImage->actualDirection = optionDir->currentIndex();
        repositoryImage->windowSize = (2 * optionWSize->currentIndex()) + 3;
        repositoryImage->badgeSize = this->referenceImage->badgeSize;
        repositoryImage->makeSelection(i, j, (i + this->referenceImage->selWidth), (j + this->referenceImage->selHeight));

        // Reemplazo file_->applyFilter();
        for(int b = 0; b < repositoryImage->badgeSize; b++) {
          for (int f = 0; f < filtrosT.size(); f++) {
            if (optionFilter.at(f)->isChecked() == true) {
              repositoryImage->applyFilter(f);
              epsilon[f] += (repositoryImage->filterValue[b][f] / repositoryImage->badgeSize);
            }
          }
        }

        double actualSimilarity = 0;
        for (int f = 0; f < filtrosT.size(); f++) {
          if (optionFilter.at(f)->isChecked() == true) {
            actualSimilarity += ((100 - ((qAbs(referenceValues[f] - epsilon[f]) * 100) / referenceValues[f])));
          }
        }
        actualSimilarity = actualSimilarity / checked;
        if (actualSimilarity > absoluteSimilarity[image]) {
          absoluteSimilarity[image] = actualSimilarity;
          coordinates->clear();
          coordinates->operator <<(QString::number(i));
          coordinates->operator <<(QString::number(j));
          coordinates->operator <<(QString::number(i + this->referenceImage->selWidth));
          coordinates->operator <<(QString::number(j + this->referenceImage->selHeight));
        }
        if (actualSimilarity == 100) {
          found = true;
        }
        j--;
      } while (j >= 0 && !found);
      qDebug() << "Fila" << i << ":" << "Similaridad" << absoluteSimilarity[image] << "% (" << time.elapsed() << "milisegundos)";
      i--;
    } while (i >= 0 && !found);
    int difference = time.elapsed();
    QBrush bgcolor = QBrush(Qt::green);
    QString per_ = QString::number(100 - double(round(absoluteSimilarity[image] * 100)) / 100);
    if (percent > 25) bgcolor = QBrush(Qt::blue);
    if (percent > 40) bgcolor = QBrush(Qt::red);
    if (percent > 50) bgcolor = QBrush(Qt::darkGray);
    result->operator <<(fileInfo->fileName());
    result->operator <<(per_);
    result->operator <<(coordinates->at(0));
    result->operator <<(coordinates->at(1));
    result->operator <<(coordinates->at(2));
    result->operator <<(coordinates->at(3));
    QTreeWidgetItem *item = new QTreeWidgetItem(*result);
    item->setForeground(1,bgcolor);
    items.append(item);
    repositoryImage->~PNGFile();
    qDebug() << "Finalizado archivo:" << fileName << "(" << difference << "milisegundos)";
  }

  resultList->clear();
  resultList->insertTopLevelItems(0, items);
  resultList->sortByColumn(1,Qt::AscendingOrder);
}
