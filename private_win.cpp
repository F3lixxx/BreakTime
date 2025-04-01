#include "private_win.h"
#include "ui_private_win.h"

private_win::private_win(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::private_win)
{
    this->setWindowState(Qt::WindowMaximized);
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
}

private_win::~private_win()
{
    delete ui;
}
