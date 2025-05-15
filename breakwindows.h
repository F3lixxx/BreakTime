#ifndef BREAKWINDOWS_H
#define BREAKWINDOWS_H

#include <QDialog>
#include <QTimer>
#include <QTime>
#include <QFile>
#include <QProcess>
#include <QDir>
#include <QFont>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QCloseEvent>
#include <QKeyEvent>
#include <QLabel>

namespace Ui {
class breakWindows;
}

class breakWindows : public QDialog
{
    Q_OBJECT

public:
    explicit breakWindows(QWidget *parent = nullptr);
    ~breakWindows();

signals:
    void timebreakstop(bool stop);

public slots:
    void timeforBreak(int time);

    void timeDown();

    void closeEvent(QCloseEvent *event) override;

private:
    Ui::breakWindows *ui;
    QTimer *timerDown;
    QTime break_updown;
    QString currentDoing;
    QVBoxLayout *lo_widgets;
    QLabel *lb_timeDown;
    QLabel *lb_todo;
    bool ok;
    int min;
};

#endif // BREAKWINDOWS_H
