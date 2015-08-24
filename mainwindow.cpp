#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_channelselector.h"
#include<QFileDialog>
#include <QStringList>
#include<QTextStream>
#include<QMessageBox>
#include "libs/edflib.h"
#include "globals.h"
#include "channelSelector.h"
#include "spikeSelector.h"
#include "about.h"
#include "header.h"
#include "help.h"
#include <QPointF>
#include "libs/CInputEDF.h"
#include "libs/CSpikeDetector.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{

  srand(QDateTime::currentDateTime().toTime_t());
  ui->setupUi(this);
  setupGraphArea();
  

}

MainWindow::~MainWindow()
{
  delete ui;
}


void MainWindow::setupGraphArea(){
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                     QCP::iSelectPlottables);

    ui->customPlot->axisRect()->setupFullAxesBox();
    // set axes ranges, so we see all data:
    ui->customPlot->xAxis->setRange(0, 10);
    ui->customPlot->yAxis->setRange(-1500, 1500);
    //ui->customPlot->yAxis->grid()->setZeroLinePen(Qt::NoPen);
    ui->customPlot->yAxis->setTickLabels(false);

    QVector<double> piTicks(60*60);
    for (int i = 0; i < piTicks.size(); ++i) {
       piTicks[i] = i;
    }

    //Set the ticks to appear at every second
    QPen gridPen;
    gridPen.setColor(QColor(0, 255, 0));
    ui->customPlot->xAxis->grid()->setPen(gridPen);
    ui->customPlot->xAxis->setScaleType(QCPAxis::stLinear);
    ui->customPlot->xAxis->setAutoTicks(false);
    ui->customPlot->xAxis->setTickVector(piTicks);
    ui->customPlot->xAxis->setSubTickCount(10);

    ui->customPlot->xAxis->setLabel("time(s)");
    ui->customPlot->yAxis->setLabel("V(uV)");


    // connect slot that ties some axis selections together (especially opposite axes):
    connect(ui->customPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
    connect(ui->customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    connect(ui->customPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

    // make bottom and left axes transfer their ranges to top and right axes:
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

    // connect some interaction slots:
    connect(ui->customPlot, SIGNAL(titleDoubleClick(QMouseEvent*,QCPPlotTitle*)), this, SLOT(titleDoubleClick(QMouseEvent*,QCPPlotTitle*)));
    connect(ui->customPlot, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)), this, SLOT(axisLabelDoubleClick(QCPAxis*,QCPAxis::SelectablePart)));

    // connect slot that shows a message in the status bar when a graph is clicked:
    connect(ui->customPlot, SIGNAL(plottableClick(QCPAbstractPlottable*,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*)));

    // setup policy and connect slot for context menu popup:
    ui->customPlot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->customPlot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));

    connect(ui->customPlot, SIGNAL(mouseMove(QMouseEvent*)), this,SLOT(showPointToolTip(QMouseEvent*)));

}

void MainWindow::titleDoubleClick(QMouseEvent* event, QCPPlotTitle* title)
{
  Q_UNUSED(event)
  // Set the plot title by double clicking on it
  bool ok;
  QString newTitle = QInputDialog::getText(this, "QCustomPlot example", "New plot title:", QLineEdit::Normal, title->text(), &ok);
  if (ok)
  {
    title->setText(newTitle);
    ui->customPlot->replot();
  }
}

void MainWindow::axisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part)
{
  // Set an axis label by double clicking on it
  if (part == QCPAxis::spAxisLabel) // only react when the actual axis label is clicked, not tick label or axis backbone
  {
    bool ok;
    QString newLabel = QInputDialog::getText(this, "QCustomPlot example", "New axis label:", QLineEdit::Normal, axis->label(), &ok);
    if (ok)
    {
      axis->setLabel(newLabel);
      ui->customPlot->replot();
    }
  }
}

void MainWindow::selectionChanged()
{
  /*
   normally, axis base line, axis tick labels and axis labels are selectable separately, but we want
   the user only to be able to select the axis as a whole, so we tie the selected states of the tick labels
   and the axis base line together. However, the axis label shall be selectable individually.
   
   The selection state of the left and right axes shall be synchronized as well as the state of the
   bottom and top axes.
   
  */
  
  // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    ui->customPlot->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    ui->customPlot->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    ui->customPlot->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    ui->customPlot->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  
}

void MainWindow::mousePress()
{
  // if an axis is selected, only allow the direction of that axis to be dragged
  // if no axis is selected, both directions may be dragged
  
  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->xAxis->orientation());
  else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->yAxis->orientation());
  else
    ui->customPlot->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void MainWindow::mouseWheel()
{
  // if an axis is selected, only allow the direction of that axis to be zoomed
  // if no axis is selected, both directions may be zoomed
  
  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->xAxis->orientation());
  else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->yAxis->orientation());
  else
    ui->customPlot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void MainWindow::addRandomGraph()
{
  int n = 50; // number of points in graph
  double xScale = (rand()/(double)RAND_MAX + 0.5)*2;
  double yScale = (rand()/(double)RAND_MAX + 0.5)*2;
  double xOffset = (rand()/(double)RAND_MAX - 0.5)*4;
  double yOffset = (rand()/(double)RAND_MAX - 0.5)*5;
  double r1 = (rand()/(double)RAND_MAX - 0.5)*2;
  double r2 = (rand()/(double)RAND_MAX - 0.5)*2;
  double r3 = (rand()/(double)RAND_MAX - 0.5)*2;
  double r4 = (rand()/(double)RAND_MAX - 0.5)*2;
  QVector<double> x(n), y(n);
  for (int i=0; i<n; i++)
  {
    x[i] = (i/(double)n-0.5)*10.0*xScale + xOffset;
    y[i] = (qSin(x[i]*r1*5)*qSin(qCos(x[i]*r2)*r4*3)+r3*qCos(qSin(x[i])*r4*2))*yScale + yOffset;
  }
  
  ui->customPlot->addGraph();
  ui->customPlot->graph()->setName(QString("RG %1").arg(ui->customPlot->graphCount()-1));
  ui->customPlot->graph()->setData(x, y);
  ui->customPlot->graph()->setLineStyle((QCPGraph::LineStyle)(rand()%5+1));
  if (rand()%100 > 50)
    ui->customPlot->graph()->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)(rand()%14+1)));
  QPen graphPen;
  graphPen.setColor(QColor(rand()%245+10, rand()%245+10, rand()%245+10));
  graphPen.setWidthF(rand()/(double)RAND_MAX*2+1);
  ui->customPlot->graph()->setPen(graphPen);
  ui->customPlot->replot();
}

void MainWindow::removeSelectedGraph()
{
  if (ui->customPlot->selectedGraphs().size() > 0)
  {
    shown.removeOne(ui->customPlot->selectedGraphs().first()->name());
    notshown.append(ui->customPlot->selectedGraphs().first()->name());
    notshown.sort();
    ui->customPlot->removeGraph(ui->customPlot->selectedGraphs().first());
    ui->customPlot->replot();
  }
}

void MainWindow::removeChannelByLabel(QCustomPlot *customPlot, QString label)
{
  int i = 0;
  int index = -1;
  while ( (customPlot->graph(i)->name() != label) && (i < customPlot->graphCount() ) ){
    i++;
  }
  if (customPlot->graph(i)->name() == label) index = i;

  if ((ui->customPlot->graph(index) != NULL) && (index >= 0))
  {
    customPlot->removeGraph(ui->customPlot->graph(index));
    customPlot->replot();
  }
}

void MainWindow::removeAllGraphs()
{
  ui->customPlot->clearGraphs();
  notshown.append(shown);
  shown.clear();
  notshown.sort();
  ui->customPlot->replot();
  totalGraphs = 0;
  QMap<QString, int>::iterator i;
  for (i = labelposition.begin(); i != labelposition.end(); ++i)
      i.value() = -1;

}

void MainWindow::contextMenuRequest(QPoint pos)
{
  QMenu *menu = new QMenu(this);
  menu->setAttribute(Qt::WA_DeleteOnClose);
  

  menu->addAction("Add random graph", this, SLOT(addRandomGraph()));
  if (ui->customPlot->selectedGraphs().size() > 0)
    menu->addAction("Remove selected graph", this, SLOT(removeSelectedGraph()));
  if (ui->customPlot->graphCount() > 0)
    menu->addAction("Remove all graphs", this, SLOT(removeAllGraphs()));

  
  menu->popup(ui->customPlot->mapToGlobal(pos));
}

void MainWindow::graphClicked(QCPAbstractPlottable *plottable)
{
  ui->statusBar->showMessage(QString("Clicked on graph '%1'.").arg(plottable->name()), 1000);
}

void MainWindow::on_actionOpen_triggered()
{
  char* filelocation;

  filenameg.clear();
  filenameg = QFileDialog::getOpenFileName(
                    this,
                    tr("Open file"),
                    "C://",
                    "EEG Files (*.edf; *.bdf; *.rec; *.EDF; *.BDF; *.REC);;All files(*.*)");
  if (filenameg != NULL)
  {
    if ((hdr.filetype >= 0) && (hdr.filetype <= 3)){
      edfclose_file(hdr.handle);
    }
    QMessageBox::information(this, tr("File opened:"),filenameg);
    filelocation = new char[filenameg.length() + 1];
    strcpy(filelocation, filenameg.toLatin1().constData());
  }
  else {
      QMessageBox::warning(this, tr("Alert"),QString("No file chosen"));
      return;
  }


  removeAllGraphs();
  shown.clear();
  notshown.clear();

  //read file and verify errors, check error code on edflib.h (if 0, no error found)
  if(edfopen_file_readonly(filelocation, &hdr, EDFLIB_READ_ALL_ANNOTATIONS))
  {
    switch(hdr.filetype)
    {
      case EDFLIB_MALLOC_ERROR                : printf("\nmalloc error\n\n");
                                                break;
      case EDFLIB_NO_SUCH_FILE_OR_DIRECTORY   : printf("\ncan not open file, no such file or directory\n\n");
                                                break;
      case EDFLIB_FILE_CONTAINS_FORMAT_ERRORS : printf("\nthe file is not EDF(+) or BDF(+) compliant\n"
                                                       "(it contains format errors)\n\n");
                                                break;
      case EDFLIB_MAXFILES_REACHED            : printf("\nto many files opened\n\n");
                                                break;
      case EDFLIB_FILE_READ_ERROR             : printf("\na read error occurred\n\n");
                                                break;
      case EDFLIB_FILE_ALREADY_OPENED         : printf("\nfile has already been opened\n\n");
                                                break;
      default                                 : printf("\nunknown error\n\n");
                                                break;
    }


    return;
  }

  hdl = hdr.handle;

  //SET HEADER:
  header.clear();
  QString name = QString(hdr.patient_name);
  QString gender = QString(hdr.gender);
  QString day = QString(hdr.startdate_day);
  QString month = QString(hdr.startdate_month);
  QString year = QString(hdr.startdate_year);
  QString equipment = QString(hdr.equipment);
  header += "<b>Name: </b>" + name + "<br>";
  header += "<b>Gender: </b>" + gender + "<br>";
  header += "<b>Date: </b>" + day + "" + month + "" + year + "<br>";
  header += "<b>Equipment: </b>" + equipment + "<br>";

  for (int i = 0;i < hdr.edfsignals;i++){
    notshown.append(QString(hdr.signalparam[i].label));
    labelchannel.insert(hdr.signalparam[i].label, i);
    labelposition.insert(hdr.signalparam[i].label, totalGraphs);
  }

  notshown.sort();
  shown.sort();

}

void MainWindow::insertSpikeGraph(QCustomPlot *customPlot, QString label){

    //---------------------------------------------------------------------------------------------
    //ORIGINAL CHANNEL DATA AREA
    double *buf;
    nsamples = (end_time - start_time)/time_interval;
    int channel = labelchannel[label];
    double nsec = hdr.datarecords_in_file;
    double totalsamples = hdr.signalparam[channel].smp_in_file;
    time_interval = nsec/totalsamples;

    //memory allocated to store data
    buf = (double *)malloc(sizeof(double[nsamples]));
    if(buf==NULL)
    {
      printf("\nmalloc error\n");
      edfclose_file(hdl);
      return;
    }

    if ((hdr.filetype < 0) || (hdr.filetype > 3)){
      QMessageBox::warning(this, tr("Alert"),QString("No file chosen"));
      return;
    }

    edfseek(hdl, channel, (long long) ( ((start_time) / ((double)hdr.file_duration / (double)EDFLIB_TIME_DIMENSION)) * ((double)hdr.signalparam[channel].smp_in_file)), EDFSEEK_SET);

    //CHECK ERROR IN READ FUNCTION
    if(edfread_physical_samples(hdl, channel, nsamples, buf) == (-1))
    {
      //show here error message TODO
      edfclose_file(hdl);
      free(buf);
      return;
    }

    QVector<double> x(nsamples), y(nsamples);
    for (int i=0; i<nsamples; ++i)
    {
      x[i] = start_time + i*time_interval; //
      y[i] = buf[i] + labelposition[label]*3000;  //
    }

    edfclose_file(hdl);
    free(buf);

    //---------------------------------------------------------------------------------------------
    //SPIKE DATA AREA
    if ((hdr.filetype >= 0) && (hdr.filetype <= 3)){
      edfclose_file(hdr.handle);
    }

    CInputEDF * model = new CInputEDF();
    CSpikeDetector * detector = NULL;
    DETECTOR_SETTINGS * detectorSettings = new DETECTOR_SETTINGS(10, 60, 3.65, 3.65, 0, 5, 4, 300, 50, 0.005, 0.12, 200); // default settings

    char* filelocation;

    if (filenameg != NULL)
    {
      filelocation = new char[filenameg.length() + 1];
      strcpy(filelocation, filenameg.toLatin1().constData());
    }

    // DISCHARGES
    CDischarges * discharges = NULL;
    // Output class containing output data from the detector.
    CDetectorOutput * output = NULL;

    model->OpenFile(filelocation);
    detector = new CSpikeDetector(model, detectorSettings);
    detector->AnalyseChannel(channel, &output, &discharges);
    model->CloseFile();

    //set spike data to the arrays
    int sz = output->m_pos.size();
    QVector<double> xspike(sz), yspike(sz);
    qDebug() << "channel:" << channel;
    for (int j = 0; j < sz; j++)
    {
        double d = output->m_pos.at(j)/time_interval - start_time/time_interval;
        int number = (int)floor(d);
        if ((number <= nsamples) && (d >= 0)  ){
            xspike[j] = output->m_pos.at(j);
            yspike[j] = y[number];
            qDebug() << output->m_pos.at(j);
        }
    }
    customPlot->addGraph();
    customPlot->graph()->setData(xspike, yspike);
    customPlot->graph()->setName("Spike:" + label);
    QPen graphPen;
    graphPen.setColor(Qt::GlobalColor::black);
    ui->customPlot->graph()->setPen(graphPen);
    ui->customPlot->graph()->setLineStyle(QCPGraph::lsNone);
    customPlot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, 9));
    ui->customPlot->replot();

    delete output;
    delete discharges;
    delete model;
    delete detector;
    delete detectorSettings;

    if (filenameg != NULL)
    {
      filelocation = new char[filenameg.length() + 1];
      strcpy(filelocation, filenameg.toLatin1().constData());
    }

    //read file and verify errors, check error code on edflib.h (if 0, no error found)
    if(edfopen_file_readonly(filelocation, &hdr, EDFLIB_READ_ALL_ANNOTATIONS))
    {
      switch(hdr.filetype)
      {
        case EDFLIB_MALLOC_ERROR                : printf("\nmalloc error\n\n");
                                                  break;
        case EDFLIB_NO_SUCH_FILE_OR_DIRECTORY   : printf("\ncan not open file, no such file or directory\n\n");
                                                  break;
        case EDFLIB_FILE_CONTAINS_FORMAT_ERRORS : printf("\nthe file is not EDF(+) or BDF(+) compliant\n"
                                                         "(it contains format errors)\n\n");
                                                  break;
        case EDFLIB_MAXFILES_REACHED            : printf("\nto many files opened\n\n");
                                                  break;
        case EDFLIB_FILE_READ_ERROR             : printf("\na read error occurred\n\n");
                                                  break;
        case EDFLIB_FILE_ALREADY_OPENED         : printf("\nfile has already been opened\n\n");
                                                  break;
        default                                 : printf("\nunknown error\n\n");
                                                  break;
      }

    }
}

void MainWindow::insertChannel(QCustomPlot *customPlot, QString label)
{
  double *buf;
  nsamples = (end_time - start_time)/time_interval;
  int channel = labelchannel[label];
  double nsec = hdr.datarecords_in_file;
  double totalsamples = hdr.signalparam[channel].smp_in_file;
  time_interval = nsec/totalsamples;

  //memory allocated to store data
  buf = (double *)malloc(sizeof(double[nsamples]));
  if(buf==NULL)
  {
    printf("\nmalloc error\n");
    edfclose_file(hdl);
    return;
  }


//  if (hdr. == NULL){
//    QMessageBox::warning(this, tr("Alert"),QString("No file chosen"));
//    return;
//  }

  edfseek(hdl, channel, (long long) ( ((start_time) / ((double)hdr.file_duration / (double)EDFLIB_TIME_DIMENSION)) * ((double)hdr.signalparam[channel].smp_in_file)), EDFSEEK_SET);

  //CHECK ERROR IN READ FUNCTION
  if(edfread_physical_samples(hdl, channel, nsamples, buf) == (-1))
  {
    //show here error message TODO
    edfclose_file(hdl);
    free(buf);
    return;
  }

  QVector<double> x(nsamples), y(nsamples);
  //SET DATA TO THE X AND Y AXES
  for (int i=0; i<nsamples; ++i)
  {
    x[i] = start_time + i*time_interval; //
    y[i] = buf[i] + totalGraphs*3000;  //
  }


  // create graph and assign data to it:
  customPlot->addGraph();
  customPlot->graph()->setData(x, y);
  customPlot->graph()->setName(label);
  QPen graphPen;
  graphPen.setColor(QColor(rand()%245+10, rand()%245+10, rand()%245+10));
  ui->customPlot->graph()->setPen(graphPen);
  ui->customPlot->yAxis->setRange(-3000,(totalGraphs+1)*3000);
  ui->customPlot->replot();

  labelposition[label] = totalGraphs;
  totalGraphs++;

  //edfclose_file(hdl);
  free(buf);

}

void MainWindow::on_actionChannel_Selector_triggered()
{

  //note the difference between the 'showngraphs' and 'shown'
  QStringList showngraphs;
  ChannelSelector channelSelector;
  channelSelector.setModal(true);
  channelSelector.exec();

  //create a list with the graphs that are shown in the plot area
  for (int i=0; i < ui->customPlot->graphCount(); i++)
  {
      showngraphs.append(ui->customPlot->graph(i)->name());
  }

  //if the graph is not show and is in the shown list, plots the graph
  foreach (QString str, shown) {
    if(!showngraphs.contains(str))  insertChannel(ui->customPlot, str);
  }
  //if the graph is shown and is in the notshown list, removes the graph
  foreach (QString str, notshown) {
    if(showngraphs.contains(str))  removeChannelByLabel(ui->customPlot, str);
  }

}

void MainWindow::on_actionSet_Time_triggered()
{
    bool ok = 1;
    QString initialtime = QInputDialog::getText(this, tr("Set initial time"),
      tr("Type initial time in seconds"), QLineEdit::Normal,
      QString(""), &ok);
    if (!initialtime.isEmpty()) start_time = initialtime.toDouble();

    QString finaltime = QInputDialog::getText(this, tr("Set final time"),
      tr("Type final time in seconds"), QLineEdit::Normal,
      QString(""), &ok);
    if (!finaltime.isEmpty()) end_time = finaltime.toDouble();

    //now we need to replot all graphs
    //so we need a copy of what is shown and not shown
    //because we will use the removeAllGraphs method and
    //it will reset the shown and notshown lists
    QStringList shownbkp = shown;
    QStringList notshownbkp = notshown;
    removeAllGraphs();
    shown = shownbkp;
    notshown = notshownbkp;
    ui->customPlot->xAxis->setRange(initialtime.toInt(), initialtime.toInt() + 10);
    if (!shown.isEmpty())
    {
      foreach (QString str, shown) {
        insertChannel(ui->customPlot, str);
      }
    }
    else ui->customPlot->replot();

}

void MainWindow::on_actionAdd_All_Channels_triggered()
{
    QStringList all;
    QStringList showngraphs;
    for (int i=0; i < ui->customPlot->graphCount(); i++)
    {
        showngraphs.append(ui->customPlot->graph(i)->name());
    }
    //put all the graphs in the shown list
    //and remove all the others from the notshown list
    all = shown;
    all.append(notshown);
    all.sort();
    shown = all;
    notshown.clear();

    //if the graph is not show yet and is in the shown list, plots the graph
    foreach (QString str, all) {
      if(!showngraphs.contains(str))  insertChannel(ui->customPlot, str);
    }
}

void MainWindow::showPointToolTip(QMouseEvent *event)
{
    double x = ui->customPlot->xAxis->pixelToCoord(event->pos().x());
    double y = ui->customPlot->yAxis->pixelToCoord(event->pos().y());
    int position;

    if (ui->customPlot->plottableAt(event->pos(), false)) {
        QString label;
        if (ui->customPlot->plottableAt(event->pos(), false)->name().contains("Spike"))
            label = ui->customPlot->plottableAt(event->pos(), false)->name().replace("Spike:","");
        else
            label = ui->customPlot->plottableAt(event->pos(), false)->name().replace("Spike:","");
        position = labelposition[label];
        y = y - 3000*position;
        QString toolLabel = ui->customPlot->plottableAt(event->pos(), false)->name().trimmed() + ": " +QString::number(x) + ", "+ QString::number(y) ;
        setToolTip(toolLabel);
    }
}

void MainWindow::on_actionSet_Display_Time_triggered()
{
    bool ok = 1;
    QString initialtime = QInputDialog::getText(this, tr("Set initial display time"),
      tr("Type initial time in seconds"), QLineEdit::Normal,
      QString(""), &ok);

    QString finaltime = QInputDialog::getText(this, tr("Set final display time"),
      tr("Type final time in seconds"), QLineEdit::Normal,
      QString(""), &ok);

    if ( !initialtime.isEmpty() && !finaltime.isEmpty() ){
        ui->customPlot->xAxis->setRange(initialtime.toInt(), finaltime.toInt());
        ui->customPlot->replot();
    }
}

void MainWindow::on_actionFilter_triggered()
{
    SpikeSelector spikeSelector;
    spikeSelector.setModal(true);
    spikeSelector.exec();

    insertSpikeGraph(ui->customPlot, spike);

}

void MainWindow::on_actionAbout_triggered()
{
    About aboutwindow;
    aboutwindow.setModal(true);
    aboutwindow.exec();
}

void MainWindow::on_actionHeader_Info_triggered()
{
    Header headerwindow;
    headerwindow.setModal(true);
    headerwindow.exec();
}

void MainWindow::on_actionHelp_triggered()
{
    Help helpwindow;
    helpwindow.setModal(true);
    helpwindow.exec();
}
