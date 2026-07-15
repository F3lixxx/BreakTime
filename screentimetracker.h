#ifndef SCREENTIMETRACKER_H
#define SCREENTIMETRACKER_H

#include <QDialog>
#include <QObject>
#include <QTimer>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QMap>
#include <QDateTime>
#include <QString>
#include <QTableWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QCoreApplication>
#include <QPushButton>
#include <QStandardPaths>
#include <QDir>
#include <windows.h>

struct HWND__;
typedef HWND__* HWND;

namespace Ui {
class screentimetracker;
}

class screentimetracker : public QDialog
{
    Q_OBJECT

public:
    explicit screentimetracker(QWidget *parent = nullptr);
    ~screentimetracker();


private slots:
    void tick();
    void commitBatch();

public slots:
    void refreshData();

private:
    Ui::screentimetracker *ui;

    void loadDataFromDb();
    QString formatDuration(qint64 totalSeconds);


    // Timers
    QTimer m_timer;
    QTimer m_commitTimer;

    //DataBase
    QSqlDatabase m_db;

    //status tracking
    QDateTime m_lastTick;
    QString m_lastProcess;
    int m_lastIntervalId = -1;


    //cash program names, for not read FileDescrition everytime
    QMap<QString, QString> m_nameCache;

    //initialize
    void initDataBase();

    //work with DataBase
    void updateDataBase(const QString &processName, const QString &displayName, qint64 deltaSec);

    QString m_currentDate; // дата, для которой сейчас открыта БД (формат YYYY-MM-DD)

    QString getDbPathForDate(const QString &date);
    void checkDateChange();// проверяет, не наступил ли новый день, и переключает БД при необходимости

    //Get info from active Window/process
    QString getProcessName(HWND hwnd, QString &outFullPath);
    QString getActiveWindowTitle(HWND hwnd);

    //Forming human-readable name
    QString humanizeAppName(const QString &processName, const QString &exeFullPath, const QString &title);
    QString getFileDescription(const QString &exePath);
    QString fallbackName(const QString &processName);
    const QMap<QString, QString>& manualOverrides();

    //check active user
    bool isUserActive(unsigned long maxIdleSeconds);


    QTableWidget *table;
    QPushButton *btnRefresh;
    QLabel *lbTotalTime;
    QVBoxLayout *layout_;
};

#endif // SCREENTIMETRACKER_H
