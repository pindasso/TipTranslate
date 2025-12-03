#ifndef LISTENERTIP_H
#define LISTENERTIP_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class ListenerTip;
}
QT_END_NAMESPACE

class ListenerTip : public QMainWindow
{
    Q_OBJECT

public:
    ListenerTip(QWidget *parent = nullptr);
    ~ListenerTip();

private:
    Ui::ListenerTip *ui;
};
#endif // LISTENERTIP_H
