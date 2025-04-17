#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , time_updown(0,0,0)
{
    min = 0;
    breakMins = 0;
    ui->setupUi(this);
    new_Win = new private_win;
    timerDown = new QTimer(this);
    connect(timerDown, SIGNAL(timeout()), this, SLOT(on_pb_open_clicked()));
    connect(this, &MainWindow::breakTime, new_Win, &private_win::timeforBreak);
}

void MainWindow::on_pb_open_clicked(){
    if (time_updown == QTime(0, 0, 0)) {
        timerDown->stop();
        new_Win->show();
        breaktimefunc();
        return;
    }

    // Уменьшаем время на одну секунду
    time_updown = time_updown.addSecs(-1);
    ui->lb_qtime->setText(time_updown.toString("mm:ss"));

    //блок кнопок и лэйблов для редавктирования
    ui->le_time->setEnabled(false);
    ui->le_break->setEnabled(false);
    ui->pb_start->setEnabled(false);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pb_start_clicked()
{

    min = ui->le_time->text().toInt(&ok);

    if (!ok || min < 0) {
        qDebug() << "Некорректное значение минут";
        ui->lb_qtime->setText("Ошибка: введите положительное число.");
        return;
    }

    // Инициализация времени и запуск таймера
    time_updown = QTime(0, min, 0);
    ui->lb_qtime->setText(time_updown.toString("m:ss"));
    timerDown->start(1000);

}

void MainWindow::breaktimefunc()
{
    breakMins = ui->le_break->text().toInt(&ok);
    if (!ok || breakMins < 0) {
        qDebug() << "Некорректное значение перерыва";
        return;
    }

    emit breakTime(breakMins);  // Отправляем сигнал
}



