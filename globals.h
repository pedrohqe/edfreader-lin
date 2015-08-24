#ifndef GLOBALS_H
#define GLOBALS_H
#include <QString>
#include <QStringList>
#include <QMap.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"


// ALL THE GLOBAL DECLARATIONS

// don't use #include <QString> here, instead do this:


// that way you aren't compiling QString into every header file you put this in...
// aka faster build times.

#define MAGIC_NUM 42

extern int hdl;
extern double start_time;
extern double end_time;
extern int nsamples;
extern int totalGraphs;
extern double time_interval;
extern QString filenameg; //stores the location of the file (g stands for global)
extern QStringList all;
extern QStringList shown;
extern QStringList notshown;
extern QString spike;
extern QString header;
extern struct edf_hdr_struct hdr;
extern QMap<QString, int> labelchannel;
extern QMap<QString, int> labelposition;
extern MainWindow *ui;

#endif // GLOBALS_H
