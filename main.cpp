#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QObject>
#include <QDebug>

QDateTime startDateTime;

void logwrite(const QString &message){
    QString logFile = QCoreApplication::applicationDirPath() + "/log.ini";
    QFile file(logFile);
    if(file.open(QIODevice::Append | QIODevice::Text)){
        QTextStream out(&file);
        out << message << "\n";
        file.close();
    } else {
        qDebug() << "Не удалось открыть лог файл.";
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString startTime = QDateTime::currentDateTime().toString("hh:mm:ss dd-MM-yyyy");

    startDateTime = QDateTime::currentDateTime();
    logwrite("Программа запущена в: " + startTime);

    // Реакция на закрытие приложения
    QObject::connect(&a, &QCoreApplication::aboutToQuit, [&](){
        QDateTime endDateTime = QDateTime::currentDateTime();

        logwrite("Программа закрыта в: " + endDateTime.toString("hh:mm:ss dd-MM-yyyy"));

        qint64 msecs = startDateTime.msecsTo(endDateTime);
        int secs = msecs / 1000;
        int hours = secs / 3600;
        int minutes = (secs % 3600) / 60;
        int seconds = secs % 60;
        QString totalTime = QString("Общее время работы программы: %1 часов, %2 минут, %3 секунд.")
                                .arg(hours)
                                .arg(minutes)
                                .arg(seconds);

        logwrite(totalTime);
    });

    QFile stylesheet_file (":/styles/qss/style.css");
    stylesheet_file.open( QFile :: ReadOnly);
    QString stylesheet = QLatin1String (stylesheet_file.readAll());
    qApp-> setStyleSheet (stylesheet);
    MainWindow w;
    w.show();
    return a.exec();
}
