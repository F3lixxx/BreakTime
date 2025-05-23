#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStatusBar>
#include <QtMultimedia/QSoundEffect>
#include <QMessageBox>
#include <QShortcut>
#include <QCloseEvent>
#include <QProcess>
#include <QSystemTrayIcon>
#include <QStyle>
#include <QAction>
#include <QString>
#include <QTimer>
#include <QTime>
#include <QIntValidator>
#include <QBrush>
#include <QPalette>
#include <QSpinBox>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include "breakwindows.h"
#include "declensionSpinBox.h"

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

protected:
    void closeEvent(QCloseEvent * event) override;

private slots:
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void setTrayIconActions();
    void showTrayIcon();

signals:
    void breakTime(int breakMin);

private slots:
    void startTimer();

    void on_pb_start_stop_clicked();

    void on_pb_reset_clicked();

    void breaktimefunc();

    void stopSignal(bool timeStop);

private:
    Ui::MainWindow *ui;
    QSoundEffect *notification;
    QMenu *trayIconMenu;
    QAction *restoreAction;
    QAction *quitAction;
    QSystemTrayIcon *trayIcon;
    QStatusBar *statusBar;
    breakWindows *breaktime;
    QTimer *timerDown;
    QTime time_updown;
    QTextEdit *te_repeats;
    declensionSpinBox *sb_repeats;
    QTextEdit *te_work_time;
    QSpinBox *sb_work_time;
    QTextEdit *te_break_time;
    QSpinBox *sb_break_time;
    QPushButton *pb_start_stop;
    QPushButton *pb_reset;
    QLabel *lb_planner;
    QLabel *lb_time;
    QLabel *lb_msgStatusBar;
    QWidget *central_widget;
    QWidget *styleForTeSb;
    QWidget *styleForBtns;
    QHBoxLayout *lo_buttons_h;
    QVBoxLayout* lo_widgetsBorder;
    QFont sizeText;
    QString todo;
    bool running = false;
    int min;
    int breakMins;
    int countTime;
};
#endif // MAINWINDOW_H
