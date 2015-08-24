#include "globals.h"
#include <QString>
#include <QStringList>
#include <QMap>
#include "libs/edflib.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

// ALL THE GLOBAL DEFINITIONS

int hdl;
double start_time = 0 ; /* start reading x seconds from start of file */
double end_time = 10;  /* end reading x seconds from start of file */
int totalGraphs = 0;
double time_interval = 0.005;
int nsamples = (end_time - start_time)/time_interval;
struct edf_hdr_struct hdr;
QVector<double> xspike;
QVector<double> yspike;
QString filenameg;
QStringList all;
QStringList shown;
QStringList notshown;
QString spike;
QString header;
QMap<QString, int> labelchannel;
QMap<QString, int> labelposition;
MainWindow *ui;





