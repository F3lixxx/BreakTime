#include "mainwindow.h"
#include <QWidget>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile stylesheet_file (":/new/stylesheets/qss/style.css");
    stylesheet_file.open( QFile :: ReadOnly);
    QString stylesheet = QLatin1String (stylesheet_file.readAll());
    qApp-> setStyleSheet (stylesheet);
    MainWindow w;
    w.show();
    return a.exec();
}
