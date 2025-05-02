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
    // ui->pb_reset->hide();
    ui->le_todo->hide();
    lo_horizontal = new QHBoxLayout(this);
    sb_count = new declensionSpinBox(this);
    te_count = new QTextEdit(this);
    te_work = new QTextEdit(this);

    //стиль для layout'ов
    // 1. Создаем контейнер
    QWidget *container = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout(container);
    QVBoxLayout *lo_vertical = new QVBoxLayout(this);

    // 2. Настраиваем виджеты
    te_count->setReadOnly(true);
    te_count->setFontPointSize(18);
    te_count->setText("Количество перерывов: ");
    te_count->setAlignment(Qt::AlignCenter);

    sb_count->setRange(1, 1440);
    sb_count->setWordForms({"раз", "раза", "разов"});

    // 3. Добавляем виджеты в layout контейнера
    layout->addWidget(te_count);
    layout->addWidget(sb_count);

    // 4. Устанавливаем имя и стиль для контейнера
    container->setObjectName("hl_work");
    container->setStyleSheet(R"(
    #hl_work {
        background-color: #333;
        border: 2px solid #4A4A4A;
        border-radius: 8px;
        padding: 10px 20px;
    }
    #hl_work QLabel {
        color: #E0E0E0;
        font-size: 16px;
        padding: 10px;
    }

    #hl_work QTextEdit,
    #hl_work QSpinBox {
        background-color: transparent; /* Убираем фон */
        border: none; /* Убираем границу */
        color: white; /* Возвращаем стандартный цвет текста */
    }
    QSpinBox::up-button,
    QSpinBox::down-button {
        background-color: #555; /* Цвет кнопок */
        width: 10px;
        height: 10px;
        border: 1px solid #777;
        border-radius: 3px;
        margin: 2px;
    }
    QSpinBox::up-arrow {
        image: url(:/qt-project.org/styles/commonstyle/images/up-16.png); /* Qt встроенная иконка вверх */
    }

    QSpinBox::down-arrow {
        image: url(:/qt-project.org/styles/commonstyle/images/down-16.png); /* Qt встроенная иконка вниз */
    }

    /* Наведение */
    QSpinBox::up-button:hover {
        background-color: #888;
    }

    QSpinBox::down-button:hover {
        background-color: #888;
    }
)");

    // 5. Добавляем ВЕСЬ контейнер в основной verticalLayout
    ui->verticalLayout->insertWidget(0, container); // insertWidget, а не insertLayout

    //сколько минут сидеть перед компом
    ui->sb_time->setSuffix(" мин");
    ui->sb_time->setRange(1, 60);
    ui->sb_time->setSingleStep(1);
    qDebug() << ui->sb_time->value();

    te_work->setReadOnly(true);
    te_work->setFontPointSize(18);
    te_work->setText("Время работы: ");
    te_work->setAlignment(Qt::AlignCenter);
    lo_vertical->addWidget(te_work);
    ui->verticalLayout->insertWidget(1, te_work);

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
