#pragma once
#include <QWidget>
#include <QPixmap>
#include <QPoint>
#include <QPushButton>

class FloatingPane : public QWidget {
    Q_OBJECT
public:
    explicit FloatingPane(const QRect& where, const QPixmap& img, QWidget* parent=nullptr);

    void setOverlayText(const QString& text);
    void setRawText(const QString& text);
protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void moveEvent(QMoveEvent* event) override;
private:
    void updateButtonBarPosition();

    QPixmap img_;
    QString translatedText_;
    QString ocrText_;

    QPoint dragStartPosGlobal_;
    QPoint windowStartPos_;

    QWidget* buttonBar_ = nullptr;
    QPushButton* closeBtn_ = nullptr;
    QPushButton* copyOcrBtn_ = nullptr;
    QPushButton* copyTrBtn_ = nullptr;
};
