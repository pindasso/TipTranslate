#include "OverlaySelect.h"
#include <QApplication>
#include <QScreen>
#include <QPainter>
#include <QMouseEvent>
#include <QGuiApplication>

OverlaySelect::OverlaySelect(QWidget* parent) : QWidget(parent) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setCursor(Qt::CrossCursor);
    setGeometry(QApplication::primaryScreen()->geometry());
    showFullScreen();
}

void OverlaySelect::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.fillRect(rect(), QColor(0,0,0,160));
    if (!selection_.isNull()) {
        p.setCompositionMode(QPainter::CompositionMode_Clear);
        p.fillRect(selection_.normalized(), Qt::transparent);
    }
}

void OverlaySelect::mousePressEvent(QMouseEvent* e) {
    if (e->button() == Qt::LeftButton) {
        origin_ = e->pos();
        selection_ = QRect(origin_, QSize());
        dragging_ = true;
        update();
    }
}

void OverlaySelect::mouseMoveEvent(QMouseEvent* e) {
    if (!dragging_) return;
    selection_ = QRect(origin_, e->pos()).normalized();
    update();
}

void OverlaySelect::mouseReleaseEvent(QMouseEvent* e) {
    if (e->button() == Qt::LeftButton && dragging_) {
        dragging_ = false;
        selection_ = selection_.normalized();
        // captura
        auto screen = QGuiApplication::primaryScreen();
        QPixmap pix = screen->grabWindow(0, selection_.x(), selection_.y(),
                                         selection_.width(), selection_.height());
        emit areaSelected(selection_, pix);
        close();
        deleteLater();
    }
}

void OverlaySelect::keyPressEvent(QKeyEvent* e) {
    if (e->key() == Qt::Key_Escape) {
        close();
        deleteLater();
    }
}
