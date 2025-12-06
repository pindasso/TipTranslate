#pragma once
#include <QWidget>
#include <QPixmap>

class FloatingPane : public QWidget {
    Q_OBJECT
public:
    FloatingPane(const QRect& where, const QPixmap& img, QWidget* parent=nullptr);
    void setOverlayText(const QString& text);
protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
private:
    QPixmap img_;
    QString text_;

    QPoint dragStartPosGlobal_;
    QPoint windowStartPos_;
};
