#pragma once
#include <QObject>
#include <QImage>
#include <QString>

class OcrWorker : public QObject {
    Q_OBJECT
public:
    explicit OcrWorker(QObject* parent=nullptr);
    void setLang(const QString& lang);
signals:
    void ocrDone(const QString& text);
public slots:
    void runOcr(const QImage& img);
private:
    QString lang_ = "eng";
};
