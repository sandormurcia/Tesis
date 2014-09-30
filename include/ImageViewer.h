#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QMainWindow>
#include <iostream>
#include <cmath>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
//#include "FilePGM.h"

using namespace cv;
using namespace std;

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
class QToolBar;
class QEvent;
class QPainter;
class QImage;
class QDockWidget;
class QGroupBox;
class QRadioButton;
class QDebug;
class QComboBox;
class QCheckBox;
class QPushButton;
class QTreeWidget;
class QTreeWidgetItem;
class QStringList;
class QModelIndex;
QT_END_NAMESPACE


class ImageViewer : public QMainWindow
{
  Q_OBJECT

public:
  ImageViewer();
  QStringList filtrosT;

private slots:
  void open();
  void closeFile();
  void selectArea();
  void processArea();
  void cancelSelection();
  void about();
  void openRepositoryImage(QTreeWidgetItem *item, int column);
  void changeFilterValues (int index);

protected:
  int *values;
  bool eventFilter(QObject *, QEvent *);

private:
  void createActions();
  void createMenus();
  void createToolBars();
  void cancelSelection(bool option);
  void createStatusBar();
  void updateActions();
  void restrictPoints();
  void searchInRepository();
  void searchEpsilonInRepository();

  QLabel *imageLabel;
  QLabel *reposLabel;
  QLabel *previewLabel;
  QScrollArea *scrollArea;
  QScrollArea *reposArea;
  QToolBar *mainToolBar;
  QDockWidget *rightPanel;
  QDockWidget *filtersPanel;
  QDockWidget *resultPanel;
  QDockWidget *repositoryViewer;
  QTreeWidget *resultList;

  QStringList elements;
  QStringList matches;

  QGroupBox *direcciones;
  QGroupBox *filtros;
  QList<QCheckBox *> optionFilter;
  QComboBox *optionDir;
  QComboBox *optionWSize;

  QList<QTreeWidgetItem *> items;

  QAction *openAct;
  QAction *closeAct;
  QAction *procArea;
  QAction *cancArea;
  QAction *exitAct;
  QAction *aboutAct;

  QMenu *fileMenu;
  QMenu *viewMenu;
  QMenu *helpMenu;

  QImage imageLoaded;
  QImage selPreview;
  QPoint startDrag;
  QPoint endDrag;


  //FilePGM pgm;
  //FilePGM file_;
  bool dragging;
};

#endif
