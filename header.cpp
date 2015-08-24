#include "header.h"
#include "ui_header.h"
#include "globals.h"

Header::Header(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Header)
{
    ui->setupUi(this);
    ui->textBrowser->setText(header);
}

Header::~Header()
{
    delete ui;
}
