#include <QApplication>
#include <QScreen>
#include <QThread>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QIcon>
#include <QSettings>
#include <QLockFile>
#include <QDir>

#include "OverlaySelect.h"
#include "FloatingPane.h"
#include "OcrWorker.h"
#include "Translator.h"
#include "HotkListener.h"

FloatingPane* currentPane = nullptr;


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QString lockFilePath = QDir::temp().absoluteFilePath("TipTranslate.lock");
    QLockFile lock(lockFilePath);

    if (!lock.tryLock()) {
        return 0;
    }


    // ====== System Tray ======
    auto *tray = new QSystemTrayIcon(&app);
    tray->setIcon(QIcon(":/icons/icons/app.png"));
    tray->setToolTip("TipTranslate - OCR/Tradução de Tela");

    QSettings settings("config.ini", QSettings::IniFormat);

    const QString ocrLang = settings.value("ocr/lang", "eng").toString();

    const QString apiKey  = settings.value("deepl/api_key").toString();
    const QString srcLang = settings.value("deepl/source", "auto").toString();
    const QString tgtLang = settings.value("deepl/target", "pt").toString();

    if (apiKey.isEmpty()) {
        qWarning() << "API KEY não encontrada no config.ini";
    }

    auto *menu = new QMenu();
    QAction *actionCapture = menu->addAction("Capturar tela (Ctrl+Shift+T)");
    QAction *actionQuit    = menu->addAction("Sair");

    tray->setContextMenu(menu);
    tray->show();

    QObject::connect(actionQuit, &QAction::triggered, &app, &QCoreApplication::quit);

    // ====== Função que inicia o fluxo de captura ======
    auto startCapture = [&]() {
        auto *overlay = new OverlaySelect();

        QObject::connect(overlay, &OverlaySelect::areaSelected, &app,
                         [&](const QRect& area, const QPixmap& grab){
                             auto *pane = new FloatingPane(area, grab);
                             currentPane = pane;

                             QObject::connect(pane, &FloatingPane::destroyed, &app, [&](){
                                 currentPane = nullptr;
                             });

                             auto *worker = new OcrWorker();
                             worker->setLang(ocrLang);

                             QThread *thr = new QThread();
                             worker->moveToThread(thr);

                             QObject::connect(thr, &QThread::started, &app, [=]{
                                 worker->runOcr(grab.toImage());
                             });

                             QObject::connect(worker, &OcrWorker::ocrDone, pane, [=](const QString& txt){
                                 // traduz
                                 auto *tr = new Translator(pane);
                                 QObject::connect(tr, &Translator::translated, pane, [=](const QString& t){
                                     pane->setOverlayText(t);
                                 });
                                 tr->setAuthKey(apiKey);
                                 tr->setTargets(srcLang, tgtLang);
                                 tr->translate(txt);
                                 thr->quit();
                             });

                             QObject::connect(thr, &QThread::finished, worker, &QObject::deleteLater);
                             QObject::connect(thr, &QThread::finished, thr, &QObject::deleteLater);

                             thr->start();

                         });
    };

    // Clicar no item do tray faz a mesma coisa que o hotkey
    QObject::connect(actionCapture, &QAction::triggered, &app, startCapture);

    // Duplo clique no ícone também dispara captura (opcional)
    QObject::connect(tray, &QSystemTrayIcon::activated, &app,
                     [&](QSystemTrayIcon::ActivationReason reason){
                         if (reason == QSystemTrayIcon::DoubleClick) {
                             startCapture();
                         }
                     });

#ifdef Q_OS_WIN
    auto *hotkey = new HotkeyListener(&app);
    app.installNativeEventFilter(hotkey);

    QObject::connect(hotkey, &HotkeyListener::hotkeyPressed, &app, startCapture);

    QObject::connect(hotkey, &HotkeyListener::closeHotkeyPressed,
         &app,
         [&](){
             if (currentPane) {
                 currentPane->close();
                 currentPane = nullptr;
             }
         }
    );

#endif

    return app.exec();
}
