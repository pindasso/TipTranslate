#ifndef HOTKEYLISTENER_H
#define HOTKEYLISTENER_H

#include <QObject>
#include <QAbstractNativeEventFilter>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

class HotkeyListener : public QObject, public QAbstractNativeEventFilter {
    Q_OBJECT
public:
    explicit HotkeyListener(QObject *parent = nullptr);
    ~HotkeyListener() override;

    // Captura eventos do SO
    bool nativeEventFilter(const QByteArray &eventType,
                           void *message,
                           qintptr *result) override;

signals:
    void hotkeyPressed();
    void closeHotkeyPressed();
};

#endif // HOTKEYLISTENER_H
