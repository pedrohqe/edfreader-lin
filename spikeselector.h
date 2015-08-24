#ifndef SPIKESELECTOR_H
#define SPIKESELECTOR_H

#include <QDialog>

namespace Ui {
class SpikeSelector;
}

class SpikeSelector : public QDialog
{
    Q_OBJECT

public:
    explicit SpikeSelector(QWidget *parent = 0);
    ~SpikeSelector();

private slots:
    void on_spikesButton_clicked();

private:
    Ui::SpikeSelector *ui;
};

#endif // SPIKESELECTOR_H
