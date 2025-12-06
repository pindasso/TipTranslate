#include "Translator.h"

#include <QNetworkRequest>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

Translator::Translator(QObject* parent)
    : QObject(parent),
    networkManager_(new QNetworkAccessManager(this))
{
    connect(networkManager_, &QNetworkAccessManager::finished,
            this, &Translator::onReplyFinished);
}

void Translator::setTargets(const QString& s, const QString& t) {
    src_ = s;
    tgt_ = t;
}

void Translator::setAuthKey(const QString& key) {
    authKey_ = key;
}

void Translator::setEndpoint(const QString& endpoint) {
    endpoint_ = endpoint;
}

void Translator::translate(const QString& text) {
    if (authKey_.isEmpty() || endpoint_.isEmpty()) {
        emit errorOccurred("DeepL API key ou endpoint não definido.");
        emit translated(text);
        return;
    }

    QUrl url(endpoint_);
    QNetworkRequest request(url);

    // Header de auth: Authorization: DeepL-Auth-Key <key>
    QByteArray authHeader = "DeepL-Auth-Key " + authKey_.toUtf8();
    request.setRawHeader("Authorization", authHeader);

    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "application/json");

    QJsonObject root;
    QJsonArray texts;
    texts.append(text);
    root.insert("text", texts);

    root.insert("target_lang", tgt_.toUpper());

    if (!src_.isEmpty() && src_.toLower() != "auto") {
        root.insert("source_lang", src_.toUpper());
    }

    QJsonDocument doc(root);
    QByteArray body = doc.toJson(QJsonDocument::Compact);

    networkManager_->post(request, body);
}

void Translator::onReplyFinished(QNetworkReply* reply) {
    // Importante: garante liberação do reply no final
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred(
            "Erro HTTP DeepL: " + reply->errorString()
            );
        return;
    }

    const QByteArray responseData = reply->readAll();
    QJsonParseError parseError{};
    QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        emit errorOccurred("Erro ao parsear JSON do DeepL.");
        return;
    }

    QJsonObject obj = doc.object();
    if (!obj.contains("translations") || !obj["translations"].isArray()) {
        emit errorOccurred("Resposta do DeepL sem campo 'translations'.");
        return;
    }

    QJsonArray translations = obj["translations"].toArray();
    if (translations.isEmpty() || !translations[0].isObject()) {
        emit errorOccurred("Lista 'translations' vazia ou inválida.");
        return;
    }

    QJsonObject first = translations[0].toObject();
    QString translatedText = first.value("text").toString();

    if (translatedText.isEmpty()) {
        emit errorOccurred("Texto traduzido vazio na resposta do DeepL.");
        return;
    }

    emit translated(translatedText);
}
