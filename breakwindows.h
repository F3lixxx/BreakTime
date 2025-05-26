#ifndef BREAKWINDOWS_H
#define BREAKWINDOWS_H

#include <QDialog>
#include <QtMultimedia/QSoundEffect>
#include <QShortcut>
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
    void KeyPessEvent(QKeyEvent *e);

private slots:
    void emergencyExit();

private:
    Ui::breakWindows *ui;
    QSoundEffect *notificationWork;
    QShortcut *ctrlShiftE;
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
