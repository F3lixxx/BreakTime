#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , time_updown(0,0,0)
{
    ui->setupUi(this);
    min = 0;
    breakMins = 0;
    countTime = 0;
    ui->pb_reset->hide();
    new_Win = new private_win;
    timerDown = new QTimer(this);
    ui->le_time->setValidator(new QIntValidator(1, 1440, this));
    ui->le_break->setValidator(new QIntValidator(1, 1440, this));
    ui->le_count->setValidator(new QIntValidator(1, 24, this));
    connect(timerDown, SIGNAL(timeout()), this, SLOT(on_pb_open_clicked()));
    connect(this, &MainWindow::breakTime, new_Win, &private_win::timeforBreak);
    connect(new_Win, &private_win::timebreakstop, this, &MainWindow::stopSignal);
}

void MainWindow::on_pb_open_clicked(){
    if (time_updown == QTime(0, 0, 0)) {
        timerDown->stop();
        breaktimefunc();
        ui->le_count->setEnabled(true);
        ui->le_time->setEnabled(true);
        ui->le_break->setEnabled(true);
        ui->pb_start->setEnabled(true);
        return;
    }

    ui->pb_start->setText("Пауза");
    ui->pb_reset->show();
    // Уменьшаем время на одну секунду
    time_updown = time_updown.addSecs(-1);
    ui->lb_qtime->setText(time_updown.toString("mm:ss"));
    running = true;

    //блок кнопок и лэйблов для редавктирования
    ui->le_count->setEnabled(false);
    ui->le_time->setEnabled(false);
    ui->le_break->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pb_start_clicked()
{
    min = ui->le_time->text().toInt(&ok);

    if (!ok || min <= 0) {
        qDebug() << "Некорректное значение минут";
        ui->lb_qtime->setText("Ошибка: введите положительное число.");
        return;
    }

    breakMins = ui->le_break->text().toInt(&ok);
    if (!ok || breakMins <= 0) {
        qDebug() << "Некорректное значение перерыва";
        ui->le_break->setText("Некорректное значение перерыва");
        return;
    }

    if(running){
        timerDown->stop();
        running = false;
    }else if(!running){
        // Инициализация времени и запуск таймера
        time_updown = QTime(0, min, 0);
        timerDown->start(1000);
    }else{
        timerDown->stop();
    }
}

void MainWindow::breaktimefunc()
{
    emit breakTime(breakMins);  // Отправляем сигнал
}

void MainWindow::stopSignal(bool timeStop)
{
    countTime = ui->le_count->text().toInt(&ok);

    if (!ok || countTime <= 0) {
        qDebug() << "Некорректное значение количества циклов";
        return;
    }

    static int currentCycle = 0;
    currentCycle++;

    qDebug() << "Цикл завершён, текущий: " << currentCycle << " из " << countTime;

    if (currentCycle < countTime) {
        // запускаем следующий рабочий сеанс
        time_updown = QTime(0, min, 0);  // min уже задан в on_pb_start
        ui->lb_qtime->setText(time_updown.toString("mm:ss"));
        timerDown->start(1000);
    } else {
        qDebug() << "Все циклы завершены!";
        currentCycle = 0;  // сбрасываем на будущее
        ui->le_count->setEnabled(true);
        ui->le_time->setEnabled(true);
        ui->le_break->setEnabled(true);
        ui->pb_start->setEnabled(true);
    }
}

void MainWindow::on_pb_reset_clicked()
{
    time_updown = QTime(0, 0, 0);
    timerDown->stop();
    ui->lb_qtime->setText(time_updown.toString("Таймер сброшен, настрой его заново"));
    ui->pb_reset->hide();
    ui->pb_start->setText("Старт");
    ui->le_count->setEnabled(true);
    ui->le_time->setEnabled(true);
    ui->le_break->setEnabled(true);
    ui->pb_start->setEnabled(true);
    running = false;
}

