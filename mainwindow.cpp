#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , time_updown(0,0,0)
{
    min = 0;
    ui->setupUi(this);
    new_Win = new private_win;
    timerDown = new QTimer(this);
    connect(timerDown, SIGNAL(timeout()), this, SLOT(on_pb_open_clicked()));
}

void MainWindow::on_pb_open_clicked(){
    if (time_updown == QTime(0, 0, 0)) {
        timerDown->stop();
        new_Win->show();
        return;
    }

    // Уменьшаем время на одну секунду
    time_updown = time_updown.addSecs(-1);
    ui->lb_qtime->setText(time_updown.toString("m:ss"));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pb_start_clicked()
{
    bool ok;
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

