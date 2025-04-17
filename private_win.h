#ifndef PRIVATE_WIN_H
#define PRIVATE_WIN_H

#include <QDialog>
#include <QTimer>
#include <QTime>

namespace Ui {
class private_win;
}

class private_win : public QDialog
{
    Q_OBJECT

public:
    explicit private_win(QWidget *parent = nullptr);
    ~private_win();

public slots:
    void timeforBreak(int time);

    void timeDown();

private:
    Ui::private_win *ui;
    QTimer *timerDown;
    QTime break_updown;
    bool ok;
    int min;
};

#endif // PRIVATE_WIN_H
