#ifndef CHANNELSELECTOR_H
#define CHANNELSELECTOR_H

#include <QDialog>

namespace Ui {
class ChannelSelector;
}

class ChannelSelector : public QDialog
{
    Q_OBJECT

public:
    explicit ChannelSelector(QWidget *parent = 0);
    ~ChannelSelector();

private slots:
    void on_insertButton_clicked();

    void on_removeButton_clicked();

private:
    Ui::ChannelSelector *ui;
};

#endif // CHANNELSELECTOR_H
