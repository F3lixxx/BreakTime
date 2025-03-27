#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    new_Win = new private_win;
}

void MainWindow::on_pb_open_clicked()
{
    int time = ui->le_time->text().toInt();
    const int min_user = 60000;
    int min;
    min = time * min_user;
    qDebug() << min;
    QTimer::singleShot(min, this, [] {
        (new private_win)->show();
    });
    QString text = "Осталось времени: ";
    ui->lb_qtime->setText(QTime::currentTime().toString("hh:mm:ss"));
}

MainWindow::~MainWindow()
{
    delete ui;
}
