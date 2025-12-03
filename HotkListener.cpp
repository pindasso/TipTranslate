#include "HotkListener.h"

HotkeyListener::HotkeyListener(QObject *parent)
    : QObject(parent)
{
#ifdef Q_OS_WIN
    // Ctrl + Shift + T -- Execução
    RegisterHotKey(nullptr, 1,
                   MOD_CONTROL | MOD_SHIFT | MOD_NOREPEAT,
                   'T');
    // ESC -- Fechar a leitura
    RegisterHotKey(nullptr, 2,
                   0, VK_ESCAPE);

#endif
}

HotkeyListener::~HotkeyListener()
{
#ifdef Q_OS_WIN
    UnregisterHotKey(nullptr, 1);
    UnregisterHotKey(nullptr, 2);
#endif
}

bool HotkeyListener::nativeEventFilter(const QByteArray &eventType,
                                       void *message,
                                       qintptr *result)
{
    Q_UNUSED(result);

#ifdef Q_OS_WIN
    if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG") {
        MSG *msg = static_cast<MSG*>(message);
        if (msg->message == WM_HOTKEY) {
            if (msg->wParam == 1) {
                emit hotkeyPressed();
            }
            if (msg->wParam == 2){
                emit closeHotkeyPressed();
            }
            return true;
        }
    }
#endif

    return false;
}
