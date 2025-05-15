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
    timerDown = new QTimer(this);
    lb_planner = new QLabel(this);
    lb_time = new QLabel(this);
    te_repeats = new QTextEdit(this);
    sb_repeats = new declensionSpinBox(this);
    te_work_time = new QTextEdit(this);
    sb_work_time = new QSpinBox(this);
    te_break_time = new QTextEdit(this);
    sb_break_time = new QSpinBox(this);
    pb_start_stop = new QPushButton(this);
    pb_reset = new QPushButton(this);
    central_widget = new QWidget(this); //центральный виджет
    lo_widgetsBorder = new QVBoxLayout();
    lo_buttons_h = new QHBoxLayout(); //Четвёртый горизонтальный layout (кнопки)

    //контейнер с виджетами
    auto makePairWidget = [&](QTextEdit *te, QSpinBox *sb, const QString& name ){
        QWidget *pairWidget = new QWidget(this);
        pairWidget->setObjectName(name);
        QHBoxLayout *pairLayout = new QHBoxLayout();
        pairLayout->addWidget(te);
        pairLayout->addWidget(sb);
        pairWidget->setLayout(pairLayout);
        return pairWidget;
    };
    //конец

    //Сеты имени для виджетов
    central_widget->setObjectName("central_widget");
    pb_start_stop->setObjectName("pb_start_stop");
    pb_reset->setObjectName("pb_reset");
    lb_planner->setObjectName("lb_planner");
    lb_time->setObjectName("lb_time");
    te_repeats->setObjectName("te_repeats");
    sb_repeats->setObjectName("sb_repeats");
    te_work_time->setObjectName("te_work_time");
    sb_work_time->setObjectName("sb_work_time");
    te_break_time->setObjectName("te_break_time");
    sb_break_time->setObjectName("sb_break_time");
    lo_buttons_h->setObjectName("lo_buttons_h");

    //Добавление виджетов
    lo_widgetsBorder->addWidget(lb_planner);
    QWidget* repeatsPair = makePairWidget(te_repeats, sb_repeats, "pairRepeats"); // название виджетов
    QWidget* workTimePair = makePairWidget(te_work_time, sb_work_time, "pairWorkTime"); // название виджетов
    QWidget* breakTimePair = makePairWidget(te_break_time, sb_break_time, "pairBreakTime"); // название виджетов

    // Добавляем их в основной лэйаут с бордерами
    lo_widgetsBorder->addWidget(repeatsPair);
    lo_widgetsBorder->addWidget(workTimePair);
    lo_widgetsBorder->addWidget(breakTimePair);
    lo_widgetsBorder->addWidget(lb_time);
    //добавление кнопок
    lo_buttons_h->addWidget(pb_start_stop);
    lo_buttons_h->addWidget(pb_reset);
    lo_widgetsBorder->addLayout(lo_buttons_h);

    central_widget->setLayout(lo_widgetsBorder); // засетование к основному виджету
    setCentralWidget(central_widget);
    //Конец добавления виджетов

    //Настройка виджетов повторения
    repeatsPair->setMinimumHeight(50);
    repeatsPair->setMaximumHeight(70);
    te_repeats->setReadOnly(true);
    te_repeats->setFontPointSize(18);
    te_repeats->setText("Количество перерывов: ");
    te_repeats->setAlignment(Qt::AlignCenter);
    te_repeats->setMinimumHeight(50);
    te_repeats->setMaximumHeight(50);

    sb_repeats->setFixedSize(120, 50);
    sb_repeats->setWordForms({"раз", "раза", "раз"});
    sb_repeats->setRange(1, 1440);
    QLineEdit *le_edit = sb_repeats->getlineEdit();
    if(le_edit){
        le_edit->setReadOnly(true);
    }
    sb_repeats->setMinimumHeight(50);
    sb_repeats->setMaximumHeight(50);
    //Конец настройки

    //Настройка виджетов время работы
    workTimePair->setMinimumHeight(50);
    workTimePair->setMaximumHeight(70);
    te_work_time->setReadOnly(true);
    te_work_time->setFontPointSize(18);
    te_work_time->setText("Время работы: ");
    te_work_time->setAlignment(Qt::AlignCenter);
    te_work_time->setMinimumHeight(50);
    te_work_time->setMaximumHeight(50);

    sb_work_time->setFixedSize(120, 50);
    sb_work_time->setSuffix("мин");
    sb_work_time->setRange(1, 60);
    sb_work_time->setSingleStep(1);
    sb_work_time->setMinimumHeight(50);
    sb_work_time->setMaximumHeight(50);
    //Конец настройки

    //Настройка виджетов время перерыва
    breakTimePair->setMinimumHeight(50);
    breakTimePair->setMaximumHeight(70);
    te_break_time->setReadOnly(true);
    te_break_time->setFontPointSize(18);
    te_break_time->setText("Время работы: ");
    te_break_time->setAlignment(Qt::AlignCenter);
    te_break_time->setMinimumHeight(50);
    te_break_time->setMaximumHeight(50);

    sb_break_time->setFixedSize(120, 50);
    sb_break_time->setSuffix("мин");
    sb_break_time->setRange(1, 60);
    sb_break_time->setSingleStep(1);
    sb_break_time->setMinimumHeight(50);
    sb_break_time->setMaximumHeight(50);
    //Конец настройки

    //Настройка виджетов главной надписи и времени
    lb_planner->setText("Планировщик работы");
    lb_planner->setAlignment(Qt::AlignCenter);
    lb_planner->setMaximumHeight(50);
    lb_time->setText("00:00");
    lb_time->setAlignment(Qt::AlignCenter);
    lb_time->setMaximumHeight(50);
    //Конец Настройки

    //Настройка виджетов кнопок
    pb_start_stop->setText("Старт");
    pb_reset->setText("Сброс");
    //Конец настройки

    //настройка запуска времени и открытие нового окна
    breaktime = new breakWindows;
    connect(timerDown, SIGNAL(timeout()), this, SLOT(startTimer()));
    connect(pb_start_stop, &QPushButton::clicked, this, &MainWindow::on_pb_start_stop_clicked);
    connect(this, &MainWindow::breakTime, breaktime, &breakWindows::timeforBreak);
    connect(breaktime, &breakWindows::timebreakstop, this, &MainWindow::stopSignal);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startTimer()
{
    if (time_updown == QTime(0, 0, 0)) {
        timerDown->stop();
        breaktimefunc();
        pb_start_stop->setText("Старт");
        pb_reset->isActiveWindow();
        sb_repeats->setEnabled(true);
        sb_work_time->setEnabled(true);
        sb_break_time->setEnabled(true);
        pb_start_stop->setEnabled(true);
        return;
    }

    pb_start_stop->setText("Пауза");
    // Уменьшаем время на одну секунду
    time_updown = time_updown.addSecs(-1);
    lb_time->setText(time_updown.toString("mm:ss"));
    running = true;

    //блок кнопок и лэйблов для редавктирования
    sb_repeats->setEnabled(false);
    sb_work_time->setEnabled(false);
    sb_break_time->setEnabled(false);
}

void MainWindow::on_pb_start_stop_clicked()
{
    min = sb_work_time->value();
    qDebug() << "minutes: " << min;

    breakMins = sb_break_time->value();

    countTime = sb_repeats->value();
    qDebug() << "time break: " << countTime;

    if(running){
        pb_start_stop->setText("Старт");
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
    emit breakTime(breakMins);
    qDebug() << "должно открыться окно";
}

void MainWindow::stopSignal(bool timeStop)
{
    static int currentCycle = 0;
    currentCycle++;

    qDebug() << "Цикл завершён, текущий: " << currentCycle << " из " << countTime;

    if (currentCycle < countTime) {
        // запускаем следующий рабочий сеанс
        time_updown = QTime(0, min, 0);  // min уже задан в on_pb_start
        lb_time->setText(time_updown.toString("mm:ss"));
        timerDown->start(1000);
    } else {
        qDebug() << "Все циклы завершены!";
        currentCycle = 0;  // сбрасываем на будущее
        sb_repeats->setEnabled(true);
        sb_work_time->setEnabled(true);
        sb_break_time->setEnabled(true);
    }
}



