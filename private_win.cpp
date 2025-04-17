#include "private_win.h"
#include "ui_private_win.h"

private_win::private_win(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::private_win)
    , break_updown(0,0,0)
{
    ui->setupUi(this);
    this->setWindowState(Qt::WindowMaximized);
    setWindowFlags(Qt::FramelessWindowHint);

    min = 0;
    timerDown = new QTimer(this);
    connect(timerDown, SIGNAL(timeout()), this, SLOT(timeDown()));
}

private_win::~private_win()
{
    delete ui;
}

void private_win::timeforBreak(int time)
{
    break_updown = QTime(0, time, 0);
    ui->lb_timeDown->setText(break_updown.toString("Осталось времени на перерыв: mm:ss"));
    timerDown->start(1000);
}

void private_win::timeDown()
{
    if (break_updown == QTime(0, 0, 0)) {
        timerDown->stop();
        this->close();
        return;
    }

    break_updown = break_updown.addSecs(-1);
    ui->lb_timeDown->setText(break_updown.toString("Осталось времени на перерыв: mm:ss"));
}
