#pragma once
#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class Translator : public QObject {
    Q_OBJECT
public:
    explicit Translator(QObject* parent = nullptr);
    void setTargets(const QString& src, const QString& tgt);
    void setAuthKey(const QString& key);

signals:
    void translated(const QString& text);
    void errorOccurred(const QString& message);

public slots:
    void translate(const QString& text);

private slots:
    void onReplyFinished(QNetworkReply* reply);

private:
    QString src_ = "auto";
    QString tgt_ = "pt";
    QString authKey_;

    QNetworkAccessManager* networkManager_;
};
