#include "private_win.h"
#include "ui_private_win.h"

private_win::private_win(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::private_win)
    , break_updown(0,0,0)
{
    ui->setupUi(this);
    min = 0;
    timerDown = new QTimer(this);
    connect(timerDown, SIGNAL(timeout()), this, SLOT(timeDown()));
}

private_win::~private_win()
{
    delete ui;
}

void private_win::timeforBreak(int time, const QString &todo)
{
    QFont sizeFont = ui->lb_timeDown->font();
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->showFullScreen();     // Полноэкранный режим
    this->show();
    this->raise();
    this->activateWindow();
    this->grabKeyboard();
    this->setCursor(Qt::BlankCursor);
    // setFocusPolicy(Qt::NoFocus);
    // setFocus();

    currentDoing = todo;
    ui->lb_todo->setText(todo);


    qDebug() << "здесь по факту долджно отрыкться окно на " << time << " минут";
    qDebug() << "todo из главного окна " << todo;
    break_updown = QTime(0, time, 0);
    sizeFont.setPixelSize(70);
    ui->lb_timeDown->setFont(sizeFont);
    ui->lb_timeDown->setText(break_updown.toString("Осталось времени на перерыв: mm:ss"));
    timerDown->start(1000);
}

void private_win::timeDown()
{
    if (break_updown == QTime(0, 0, 0)) {
        timerDown->stop();
        this->close();
        this->releaseKeyboard();
        this->unsetCursor();
        emit timebreakstop(true);
        return;
    }

    break_updown = break_updown.addSecs(-1);
    ui->lb_timeDown->setText(break_updown.toString("Осталось времени на перерыв: mm:ss"));
}

void private_win::closeEvent(QCloseEvent *event){
    if(break_updown == QTime(0, 0, 0)){
        event->accept();
        this->close();
    }
    else{
        event->ignore();
    }
}

