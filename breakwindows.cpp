#include "breakwindows.h"
#include "ui_breakwindows.h"

breakWindows::breakWindows(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::breakWindows)
{
    ui->setupUi(this);
    min = 0;
    timerDown = new QTimer(this);
    lb_timeDown = new QLabel(this);
    lo_widgets = new QVBoxLayout();

    lo_widgets->addWidget(lb_timeDown);
    setLayout(lo_widgets);

    lb_timeDown->setAlignment(Qt::AlignCenter);

    connect(timerDown, SIGNAL(timeout()), this, SLOT(timeDown()));
}

breakWindows::~breakWindows()
{
    delete ui;
}

void breakWindows::timeforBreak(int time)
{
    QFont sizeFont = lb_timeDown->font();
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->showFullScreen();     // Полноэкранный режим
    this->show();
    this->raise();
    this->activateWindow();
    this->grabKeyboard();
    this->setCursor(Qt::BlankCursor);
    setFocusPolicy(Qt::NoFocus);
    setFocus();

    qDebug() << "здесь по факту долджно открыться окно на " << time << " минут";
    break_updown = QTime(0, time, 0);
    sizeFont.setPixelSize(70);
    lb_timeDown->setFont(sizeFont);
    lb_timeDown->setText(break_updown.toString("Осталось времени на перерыв: mm:ss"));
    timerDown->start(1000);
}

void breakWindows::timeDown()
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
    lb_timeDown->setText(break_updown.toString("Осталось времени на перерыв: mm:ss"));
}

void breakWindows::closeEvent(QCloseEvent *event){
    if(break_updown == QTime(0, 0, 0)){
        event->accept();
        this->close();
    }
    else{
        event->ignore();
    }
}
