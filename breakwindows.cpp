#include "breakwindows.h"
#include "ui_breakwindows.h"

breakWindows::breakWindows(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::breakWindows)
{
    ui->setupUi(this);
    min = 0;
    timerDown = new QTimer(this);
    notificationWork = new QSoundEffect(this);
    lb_timeDown = new QLabel(this);
    lo_widgets = new QVBoxLayout();

    //Настройка Оповещения
    notificationWork->setSource(QUrl("qrc:/qss/notification.wav"));
    //конец настройки

    //Настройка добавления виджетов
    lo_widgets->addWidget(lb_timeDown);
    setLayout(lo_widgets);
    //конец настройки

    lb_timeDown->setAlignment(Qt::AlignCenter); // лэйбл с таймером по центру
    //Настройка аварийного выхода из программы
    ctrlShiftE = new QShortcut(QKeySequence("CTRL+SHIFT+E"), this);
    connect(ctrlShiftE, SIGNAL(activated()), this, SLOT(emergencyExit()));
    //конец настройки
    connect(timerDown, SIGNAL(timeout()), this, SLOT(timeDown())); // запуск времени
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

    break_updown = QTime(0, time, 0);
    sizeFont.setPixelSize(70);
    lb_timeDown->setFont(sizeFont);
    lb_timeDown->setText(break_updown.toString("Осталось времени на перерыв: mm:ss"));
    timerDown->start(1000);
}

void breakWindows::timeDown()
{
    bool isSoundPlay = false;
    if (break_updown == QTime(0, 0, 0)) {
        timerDown->stop();
        this->close();
        this->releaseKeyboard();
        this->unsetCursor();
        emit timebreakstop(true);
        return;
    }

    if (break_updown == QTime(0,0,5) && !isSoundPlay){
        notificationWork->play();
        notificationWork->setVolume(0.5);
        isSoundPlay = true;
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

void breakWindows::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape){
        return;
    }
    QDialog::keyPressEvent(event);
}

void breakWindows::emergencyExit() // Экстренный выход из программы  надо ли оно, пока что под вопросом
{
    QMessageBox::StandardButton emergencyEx;
    emergencyEx = QMessageBox::warning(this,"Экстренный Выход", "Приложение полностью закроется и вам придется настраивать все заново, Вы уверены что хотите выйти?", QMessageBox::Yes | QMessageBox::No);
    if(emergencyEx == QMessageBox::Yes){
        qApp->exit();
    }
}

