#include "FloatingPane.h"
#include <QPainter>
#include <QGuiApplication>
#include <QScreen>
#include <QKeyEvent>
#include <QTextOption>
#include <QFontMetrics>

FloatingPane::FloatingPane(const QRect& where, const QPixmap& img, QWidget* parent)
    : QWidget(parent), img_(img) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setGeometry(where);
    show();
}

void FloatingPane::setOverlayText(const QString& text) {
    text_ = text;
    update();
}

void FloatingPane::paintEvent(QPaintEvent*) {
    QPainter p(this);

    p.drawPixmap(0, 0, width(), height(), img_);

    if (text_.isEmpty())
        return;

    p.setPen(Qt::white);
    p.setRenderHint(QPainter::TextAntialiasing, true);

    const int margin = 8;
    QRect maxRect = rect().adjusted(margin, margin, -margin, -margin);

    QRect textRect = p.boundingRect(
        maxRect,
        Qt::TextWordWrap | Qt::AlignLeft | Qt::AlignTop,
        text_
        );

    textRect = textRect.intersected(maxRect);
    QRect bgRect = textRect.adjusted(-4, -4, 4, 4);
    p.fillRect(bgRect, QColor(0, 0, 0, 140));

    // desenha o texto dentro do textRect
    QTextOption opt;
    opt.setWrapMode(QTextOption::WordWrap);
    opt.setAlignment(Qt::AlignLeft | Qt::AlignTop);
    p.drawText(textRect, text_, opt);
}
