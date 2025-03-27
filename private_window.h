#ifndef PRIVATE_WINDOW_H
#define PRIVATE_WINDOW_H

#include <QObject>

class private_Window : public QObject
{
    Q_OBJECT
public:
    explicit private_Window(QObject *parent = nullptr);

signals:
};

#endif // PRIVATE_WINDOW_H
