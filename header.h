#ifndef HEADER_H
#define HEADER_H

#include <QDialog>

namespace Ui {
class Header;
}

class Header : public QDialog
{
    Q_OBJECT

public:
    explicit Header(QWidget *parent = 0);
    ~Header();

private:
    Ui::Header *ui;
};

#endif // HEADER_H
