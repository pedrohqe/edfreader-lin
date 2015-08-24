#include "spikeselector.h"
#include "ui_spikeselector.h"
#include "globals.h"

SpikeSelector::SpikeSelector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SpikeSelector)
{
    ui->setupUi(this);
    ui->spikesBox->addItems(shown);
}


SpikeSelector::~SpikeSelector()
{
    delete ui;
}


void SpikeSelector::on_spikesButton_clicked()
{
    if (!ui->spikesBox->currentText().isEmpty()){
        spike = ui->spikesBox->currentText();
    }
    this->close();
}
