#include "listenertip.h"
#include "./ui_listenertip.h"

ListenerTip::ListenerTip(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ListenerTip)
{
    ui->setupUi(this);
}

ListenerTip::~ListenerTip()
{
    delete ui;
}
