#ifndef PRIVATE_WIN_H
#define PRIVATE_WIN_H

#include <QDialog>

namespace Ui {
class private_win;
}

class private_win : public QDialog
{
    Q_OBJECT

public:
    explicit private_win(QWidget *parent = nullptr);
    ~private_win();

private:
    Ui::private_win *ui;
};

#endif // PRIVATE_WIN_H
