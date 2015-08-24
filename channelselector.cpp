#include "channelselector.h"
#include "ui_channelselector.h"
#include "globals.h"


ChannelSelector::ChannelSelector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChannelSelector)
{
    ui->setupUi(this);
    ui->insertBox->addItems(notshown);
    ui->removeBox->addItems(shown);
}

ChannelSelector::~ChannelSelector()
{
    delete this->ui;
}

void ChannelSelector::on_insertButton_clicked()
{
    if (!ui->insertBox->currentText().isEmpty()){
        shown.append(ui->insertBox->currentText());
        notshown.removeOne(ui->insertBox->currentText());
        shown.sort();
        notshown.sort();
        ui->insertBox->clear();
        ui->removeBox->clear();
        ui->removeBox->addItems(shown);
        ui->insertBox->addItems(notshown);
    }
}

void ChannelSelector::on_removeButton_clicked()
{
    if (!ui->removeBox->currentText().isEmpty()){
        notshown.append(ui->removeBox->currentText());
        shown.removeOne(ui->removeBox->currentText());
        shown.sort();
        notshown.sort();
        ui->insertBox->clear();
        ui->removeBox->clear();
        ui->removeBox->addItems(shown);
        ui->insertBox->addItems(notshown);
    }
}
