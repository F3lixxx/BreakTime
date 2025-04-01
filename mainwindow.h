#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTime>
#include "private_win.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void newWindow();


private slots:
    void on_pb_open_clicked();

    void on_pb_start_clicked();

private:
    Ui::MainWindow *ui;
    private_win *new_Win;
    QTimer *timerDown;
    QTime time_updown;
    int min;
};
#endif // MAINWINDOW_H
