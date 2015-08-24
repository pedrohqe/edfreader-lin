#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QInputDialog>
#include "libs/qcustomplot.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  
private slots:
  void setupGraphArea();
  void titleDoubleClick(QMouseEvent *event, QCPPlotTitle *title);
  void axisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part);
  void selectionChanged();
  void mousePress();
  void mouseWheel();
  void addRandomGraph();
  void removeSelectedGraph();
  void removeAllGraphs();
  void contextMenuRequest(QPoint pos);
  void graphClicked(QCPAbstractPlottable *plottable);
  void on_actionOpen_triggered();
  void insertChannel(QCustomPlot *customPlot, QString Label);
  void insertSpikeGraph(QCustomPlot *customPlot, QString label);
  void removeChannelByLabel(QCustomPlot *customPlot, QString label);
  void on_actionChannel_Selector_triggered();
  void on_actionSet_Time_triggered();
  void on_actionAdd_All_Channels_triggered();
  void showPointToolTip(QMouseEvent *event);
  void on_actionSet_Display_Time_triggered();
  void on_actionFilter_triggered();


  void on_actionAbout_triggered();

  void on_actionHeader_Info_triggered();

  void on_actionHelp_triggered();

private:
  Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
