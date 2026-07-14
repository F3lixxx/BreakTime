#include "screentimetracker.h"
#include "ui_screentimetracker.h"

screentimetracker::screentimetracker(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::screentimetracker)
{
    ui->setupUi(this);

    setWindowTitle("Экранное время");
    resize(500, 400);

    layout_ = new QVBoxLayout(this);

    lbTotalTime = new QLabel(this);
    lbTotalTime->setAlignment(Qt::AlignCenter);
    lbTotalTime->setStyleSheet("font-size: 16px; font-weight: bold;");

    table = new QTableWidget(this);
    table->setColumnCount(2);
    table->setHorizontalHeaderLabels({"Программа", "Время использования"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);


    btnRefresh = new QPushButton("Обновить", this);
    btnRefresh->setObjectName("btn_refresh");
    connect(btnRefresh, &QPushButton::clicked, this, &screentimetracker::refreshData);

    layout_->addWidget(lbTotalTime);
    layout_->addWidget(table);
    layout_->addWidget(btnRefresh);

    initDataBase();

    loadDataFromDb();

    m_lastTick = QDateTime::currentDateTime();

    connect(&m_timer, &QTimer::timeout, this, &screentimetracker::tick);
    m_timer.start(2000);

    connect(&m_commitTimer, &QTimer::timeout, this, &screentimetracker::commitBatch);
    m_commitTimer.start(30000);

    m_db.transaction();
    qDebug();
}

screentimetracker::~screentimetracker()
{
    // if(m_db.isOpen())
    // {
    //     m_db.commit();
    //     m_db.close();
    // }
    delete ui;
}

void screentimetracker::tick()
{
    QDateTime now = QDateTime::currentDateTime();
    qint64 elapsedSec = m_lastTick.secsTo(now);
    m_lastTick = now;

    QString currentApp;
    QString currentProcess;

    if(elapsedSec > 15)
    {
        currentApp = "Sleep";
        currentProcess = "__system_idle__";
    }
    else if(!isUserActive(60))
    {
        currentApp = "Moved away";
        currentProcess = "__afk__";
    }
    else
    {
        HWND hwnd = GetForegroundWindow();
        QString fullPath;
        currentProcess = getProcessName(hwnd, fullPath);
        QString rawTitle = getActiveWindowTitle(hwnd);
        currentApp = humanizeAppName(currentProcess, fullPath, rawTitle);
    }

    if(elapsedSec <= 0)
    {
        elapsedSec = 2;
    }
    if(elapsedSec > 3600)
    {
        elapsedSec = 3600;
    }

    updateDataBase(currentProcess, currentApp, elapsedSec);
}

void screentimetracker::commitBatch()
{
    m_db.commit();
    m_db.transaction();
}

void screentimetracker::initDataBase()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    // m_db.setDatabaseName(QCoreApplication::applicationDirPath() + "/screentime.db");
    QString appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(appDataDir); // создаёт папку, если её ещё нет
    m_db.setDatabaseName(appDataDir + "/screentime.db");

    if(!m_db.open())
    {
        qDebug() << ("Data Base is not open!") << m_db.lastError().text();
    }

    QSqlQuery pragmaQuery(m_db);
    if (!pragmaQuery.exec("PRAGMA journal_mode=WAL;")) {
        qDebug() << "Не удалось включить WAL:" << pragmaQuery.lastError().text();
    }

    QSqlQuery query;
    QString createTable = "CREATE TABLE IF NOT EXISTS screen_intervals ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "process_name TEXT, "
                          "app_name TEXT, "
                          "start_time DATETIME, "
                          "end_time DATETIME, "
                          "duration_seconds INTEGER)";

    if(!query.exec(createTable))
    {
        qDebug() << "Table is not create" << query.lastError().text();
    }
}


void screentimetracker::updateDataBase(const QString &processName, const QString &displayName, qint64 deltaSec)
{
    QString currentTimeStr = QDateTime::currentDateTime().toString(Qt::ISODate);
    if(processName == m_lastProcess && m_lastIntervalId != -1)
    {
        QSqlQuery query;
        query.prepare("UPDATE screen_intervals SET "
                      "end_time = :end, "
                      "app_name = :name, "
                      "duration_seconds = duration_seconds + :delta "
                      "WHERE id = :id"
                      );

        query.bindValue(":end", currentTimeStr);
        query.bindValue(":name", displayName);
        query.bindValue(":delta", static_cast<qint64>(deltaSec));
        query.bindValue(":id", m_lastIntervalId);

        if(!query.exec())
        {
            qDebug() << "Error Update session: " << query.lastError().text();
        }
    }
    else {
        QSqlQuery query;
        query.prepare("INSERT INTO screen_intervals (process_name, app_name, start_time, end_time, duration_seconds) "
                      "VALUES (:proc, :name, :start, :end, :delta)");
        query.bindValue(":proc", processName);
        query.bindValue(":name", displayName);
        query.bindValue(":start", currentTimeStr);
        query.bindValue(":end", currentTimeStr);
        query.bindValue(":delta", static_cast<qint64>(deltaSec));

        if (query.exec()) {
            m_lastIntervalId = query.lastInsertId().toInt();
            m_lastProcess = processName;
            qDebug() << "Переключение -> Новая сессия:" << displayName;
        } else {
            qDebug() << "Ошибка создания новой сессии в БД:" << query.lastError().text();
        }
    }
}

QString screentimetracker::getProcessName(HWND hwnd, QString &outFullPath)
{
    outFullPath.clear();
    if(!hwnd) return "__unknown__";

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if(pid == 0) return "__unknownID__";

    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, false, pid);
    if(!hProc) return "__unknowdProcess__";

    wchar_t path[MAX_PATH] = L"";
    DWORD size = MAX_PATH;
    QString result = "__unknownPATH__";

    if(QueryFullProcessImageNameW(hProc, 0, path, &size))
    {
        QString fullPath = QString::fromWCharArray(path);
        outFullPath = fullPath;
        result = fullPath.section('\\', -1);
    }

    CloseHandle(hProc);
    return result;
}

QString screentimetracker::getActiveWindowTitle(HWND hwnd)
{
    if(!hwnd) return "";

    int length = GetWindowTextLength(hwnd);
    if(length == 0) return "__error length__";

    std::vector<wchar_t> buffer(length +1);
    GetWindowTextW(hwnd, buffer.data(), length + 1);

    return QString::fromWCharArray(buffer.data());
}

const QMap<QString, QString>& screentimetracker::manualOverrides() {
    // Используется только как fallback/переопределение, когда FileDescription
    // пустой, вводит в заблуждение, или для служебных псевдо-процессов трекера.
    static const QMap<QString, QString> map = {
                                               {"__system_idle__", "Сон / простой системы"},
                                               {"__afk__",         "Отошел от ПК"},
                                               {"__unknown__",     "Неизвестное приложение"},

                                               // Примеры программ, где метаданные exe часто отсутствуют или неинформативны
                                               {"cmd.exe",         "Командная строка"},
                                               {"explorer.exe",    "Проводник"},
                                               };
    return map;
}

QString screentimetracker::getFileDescription(const QString &exePath) {
    if (exePath.isEmpty()) return "";

    std::wstring path = exePath.toStdWString();

    DWORD dummy = 0;
    DWORD size = GetFileVersionInfoSizeW(path.c_str(), &dummy);
    if (size == 0) return "";

    std::vector<BYTE> buffer(size);
    if (!GetFileVersionInfoW(path.c_str(), 0, size, buffer.data())) {
        return "";
    }

    struct LANGANDCODEPAGE {
        WORD wLanguage;
        WORD wCodePage;
    } *lpTranslate = nullptr;

    UINT cbTranslate = 0;
    if (!VerQueryValueW(buffer.data(), L"\\VarFileInfo\\Translation",
                        reinterpret_cast<LPVOID*>(&lpTranslate), &cbTranslate) ||
        cbTranslate == 0) {
        return "";
    }

    wchar_t subBlock[64];
    swprintf(subBlock, 64, L"\\StringFileInfo\\%04x%04x\\FileDescription",
             lpTranslate[0].wLanguage, lpTranslate[0].wCodePage);

    LPVOID descValue = nullptr;
    UINT descLen = 0;
    if (VerQueryValueW(buffer.data(), subBlock, &descValue, &descLen) && descLen > 0) {
        return QString::fromWCharArray(static_cast<wchar_t*>(descValue));
    }

    return "";
}

QString screentimetracker::fallbackName(const QString &processName) {
    QString name = processName;
    name.remove(".exe", Qt::CaseInsensitive);
    if (name.isEmpty()) return "Неизвестное приложение";

    name.replace('_', ' ');
    name.replace('-', ' ');
    name[0] = name[0].toUpper();
    return name;
}

QString screentimetracker::humanizeAppName(const QString &processName, const QString &exeFullPath, const QString &title) {
    QString lowerTitle = title.toLower();

    // YouTube определяем по заголовку отдельно, независимо от браузера
    if (lowerTitle.contains("youtube")) {
        QString cleanTitle = title;
        cleanTitle.replace(" - YouTube - Google Chrome", "");
        cleanTitle.replace(" — YouTube — Google Chrome", "");
        cleanTitle.replace(" - YouTube", "");
        cleanTitle.replace(" — YouTube", "");
        return "YouTube: " + cleanTitle.trimmed();
    }

    QString procLower = processName.toLower();

    // 1. Ручные исключения / служебные процессы трекера
    const auto &overrides = manualOverrides();
    auto itOverride = overrides.find(procLower);
    if (itOverride != overrides.end()) {
        return itOverride.value();
    }

    // 2. Кэш — не читаем файл повторно, если уже определяли имя для этого процесса
    auto itCache = m_nameCache.find(procLower);
    if (itCache != m_nameCache.end()) {
        return itCache.value();
    }

    // 3. Пробуем достать имя из метаданных самого exe (FileDescription)
    QString desc = getFileDescription(exeFullPath);
    QString finalName = !desc.isEmpty() ? desc : fallbackName(processName);

    m_nameCache.insert(procLower, finalName);
    return finalName;
}

bool screentimetracker::isUserActive(unsigned long maxIdleSeconds) {
    LASTINPUTINFO lii;
    lii.cbSize = sizeof(LASTINPUTINFO);

    if (GetLastInputInfo(&lii)) {
        DWORD currentTime = GetTickCount();
        DWORD idleTimeSec = (currentTime - lii.dwTime) / 1000;
        return idleTimeSec < maxIdleSeconds;
    }
    return true;
}

void screentimetracker::refreshData() {
    loadDataFromDb();
}

void screentimetracker::loadDataFromDb() {
    QString connectionName = "stats_connection";

    QSqlDatabase db;
    if (QSqlDatabase::contains(connectionName)) {
        db = QSqlDatabase::database(connectionName);
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/screentime.db");
    }

    if (!db.open()) {
        qDebug() << "Не удалось открыть БД для статистики:" << db.lastError().text();
        lbTotalTime->setText("Ошибка загрузки данных");
        return;
    }

    QSqlQuery query(db);
    bool ok = query.exec(
        "SELECT app_name, SUM(duration_seconds) AS total "
        "FROM screen_intervals "
        "GROUP BY app_name "
        "ORDER BY total DESC"
        );

    if (!ok) {
        qDebug() << "Ошибка запроса статистики:" << query.lastError().text();
        lbTotalTime->setText("Ошибка загрузки данных");
        db.close();
        return;
    }

    table->setRowCount(0);
    qint64 grandTotal = 0;
    int row = 0;

    while (query.next()) {
        QString appName = query.value("app_name").toString();
        qint64 totalSec = query.value("total").toLongLong();
        grandTotal += totalSec;

        table->insertRow(row);
        table->setItem(row, 0, new QTableWidgetItem(appName));
        table->setItem(row, 1, new QTableWidgetItem(formatDuration(totalSec)));
        row++;
    }

    lbTotalTime->setText("Общее время: " + formatDuration(grandTotal));

    db.close();
}

QString screentimetracker::formatDuration(qint64 totalSeconds) {
    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int seconds = totalSeconds % 60;

    return QString("%1ч %2мин %3сек")
        .arg(hours)
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}

