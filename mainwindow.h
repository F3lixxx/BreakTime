#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QTimer>
#include <QTime>
#include <QIntValidator>
#include <QBrush>
#include <QPalette>
#include <QSpinBox>
#include <QTextEdit>
#include <QHBoxLayout>
#include "declensionspinbox.h"
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

signals:
    void breakTime(int breakMin, QString todo);


private slots:
    void on_pb_open_clicked();

    void on_pb_start_clicked();

    void breaktimefunc();

    void stopSignal(bool timeStop);

    void on_pb_reset_clicked();

private:
    Ui::MainWindow *ui;
    private_win *new_Win;
    QTimer *timerDown;
    QTime time_updown;
    declensionSpinBox *sb_count;
    QTextEdit *te_count;
    QHBoxLayout *lo_horizontal;
    QFont sizeText;
    QString todo;
    bool running = false;
    int min;
    int breakMins;
    int countTime;
};
#endif // MAINWINDOW_H
