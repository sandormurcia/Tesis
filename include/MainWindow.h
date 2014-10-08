#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmath.h>
#include <QMainWindow>
#include <QApplication>
#include <QDebug>
#include <QLabel>
#include <QScrollArea>
#include <QDockWidget>
#include <QDesktopWidget>
#include <QComboBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QEvent>
#include <QMouseEvent>
#include <QPen>
#include <QPainter>
#include <include/PNGFile.h>

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
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
  int values[9];
  bool eventFilter(QObject *, QEvent *);

private:
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();
  void updateActions();
  void cancelSelection(bool option);
  void restrictPoints();
  void searchInRepository();
  void searchEpsilonInRepository();

  QLabel *referenceLabel;
  QLabel *repositoryLabel;
  QScrollArea *scrollArea;
  QScrollArea *reposArea;
  QToolBar *mainToolBar;
  QDockWidget *filtersPanel;
  QDockWidget *resultPanel;
  QDockWidget *repositoryViewer;
  QTreeWidget *resultList;

  QStringList elements;
  QStringList matches;

  QGroupBox *directions;
  QGroupBox *filtersGroup;
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

  PNGFile *referenceImage;
  bool dragging;
};

#endif // MAINWINDOW_H
