
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
    te_work_time = ui->te_time;
    te_break_time = ui->te_break;
    sb_work_time = ui->sb_time;
    sb_break_time = ui->sb_break;
    pb_start_stop = ui->pb_start;
    pb_reset = ui->pb_reset;
    lb_planner = ui->lb_planner;
    lb_time = ui->lb_qtime;
    ui->le_todo->hide();
    sb_count = new declensionSpinBox(this);
    te_count = new QTextEdit(this);

    //стиль для layout'ов
    // 1. Создаем контейнер
    QWidget *container = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout(container);
    QWidget *container_hor = new QWidget;
    lo_horizontal = new QHBoxLayout(container_hor);
    QWidget *container_vert = new QWidget;
    QVBoxLayout *lo_vertical = new QVBoxLayout(container_vert);

    // 2. Настраиваем виджеты
    te_count->setReadOnly(true);
    te_count->setFontPointSize(18);
    te_count->setText("Количество перерывов: ");
    te_count->setAlignment(Qt::AlignCenter);
    te_count->setMinimumSize(500, 60);

    sb_count->setFixedSize(120, 60);
    sb_count->setWordForms({"раз", "раза", "раз"});
    sb_count->setRange(1, 1440);
    QLineEdit *le_edit = sb_count->getlineEdit();
    if(le_edit){
         le_edit->setReadOnly(true);
    }
    sb_count->setMaximumSize(1000,100);

    // 3. Добавляем виджеты в layout контейнера
    layout->addWidget(te_count);
    layout->addWidget(sb_count);
    lo_horizontal->addWidget(te_work_time);
    lo_horizontal->addWidget(sb_work_time);
    lo_vertical->addWidget(te_break_time);

    // 4. Устанавливаем имя и стиль для контейнера
    container->setObjectName("count_break");
    container_vert->setObjectName("wigdets");

    // 5. Добавляем ВЕСЬ контейнер в основной verticalLayout
    ui->verticalLayout->insertWidget(0, container); // insertWidget, а не insertLayout
    ui->verticalLayout->insertWidget(1, container_hor);
    ui->verticalLayout->insertWidget(2, container_vert);

    //сколько минут сидеть перед компом
    ui->sb_time->setSuffix(" мин");
    ui->sb_time->setRange(1, 60);
    ui->sb_time->setSingleStep(1);
    qDebug() << ui->sb_time->value();

    // //сколько минут сделать перерыв
    ui->sb_break->setSuffix(" мин");
    ui->sb_break->setRange(1, 60);
    ui->sb_break->setSingleStep(1);
    qDebug() << ui->sb_break->value();

    new_Win = new private_win;
    timerDown = new QTimer(this);
    connect(timerDown, SIGNAL(timeout()), this, SLOT(on_pb_open_clicked()));
    connect(this, &MainWindow::breakTime, new_Win, &private_win::timeforBreak);
    connect(new_Win, &private_win::timebreakstop, this, &MainWindow::stopSignal);
}

void MainWindow::on_pb_open_clicked(){
    if (time_updown == QTime(0, 0, 0)) {
        timerDown->stop();
        breaktimefunc();
        ui->pb_start->setText("Старт");
        ui->pb_reset->hide();
        sb_count->setEnabled(true);
        ui->sb_time->setEnabled(true);
        ui->sb_break->setEnabled(true);
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
    sb_count->setEnabled(false);
    ui->sb_time->setEnabled(false);
    ui->sb_break->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pb_start_clicked()
{
    min = ui->sb_time->value();
    qDebug() << "minutes: " << min;
    if (min <= 0) {
        qDebug() << "Некорректное значение минут";
        ui->lb_qtime->setText("Ошибка: введите положительное число.");
        return;
    }

    breakMins = ui->sb_break->value();
    if (breakMins <= 0) {
        qDebug() << "Некорректное значение перерыва";
        ui->lb_qtime->setText("Некорректное значение перерыва");
        return;
    }

    todo = ui->le_todo->text();

    countTime = sb_count->value();
    qDebug() << "time break: " << countTime;

    if (countTime <= 0) {
        qDebug() << "Некорректное значение количества циклов";
        ui->lb_qtime->setText("Некорректное значение циклов");
        return;
    }

    if(running){
        ui->pb_start->setText("Старт");
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
    emit breakTime(breakMins, todo);  // Отправляем сигнал
    qDebug() << "minutes" << breakMins << "  \n todo list in main: " << todo;
}

void MainWindow::stopSignal(bool timeStop)
{

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
        sb_count->setEnabled(true);
        ui->sb_time->setEnabled(true);
        ui->sb_break->setEnabled(true);
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
    sb_count->setEnabled(true);
    ui->sb_time->setEnabled(true);
    ui->sb_break->setEnabled(true);
    ui->pb_start->setEnabled(true);
    running = false;
}
