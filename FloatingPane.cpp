#include "FloatingPane.h"
#include <QPainter>
#include <QGuiApplication>
#include <QScreen>
#include <QKeyEvent>
#include <QTextOption>
#include <QFontMetrics>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMoveEvent>
#include <QClipboard>


FloatingPane::FloatingPane(const QRect& where, const QPixmap& img, QWidget* parent)
    : QWidget(parent), img_(img) {

    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setGeometry(where);
    show();

    buttonBar_ = new QWidget(nullptr);
    buttonBar_->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    buttonBar_->setAttribute(Qt::WA_TranslucentBackground, true);

    auto layout = new QHBoxLayout(buttonBar_);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(4);

    auto closeBtn = new QPushButton("X", buttonBar_);
    closeBtn->setFixedSize(24, 24);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(
        "QPushButton {"
        "  border: 1px solid #555;"
        "  border-radius: 12px;"
        "  background-color: rgba(0, 0, 0, 180);"
        "  color: white;"
        "  font-weight: bold;"
        "  padding: 0;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgba(255, 0, 0, 220);"
        "}"
        );
    layout->addWidget(closeBtn);

    // Copiar RAW
    copyOcrBtn_ = new QPushButton("RAW", buttonBar_);
    copyOcrBtn_->setFixedSize(40, 24);
    copyOcrBtn_->setCursor(Qt::PointingHandCursor);
    layout->addWidget(copyOcrBtn_);
    copyOcrBtn_->hide();

    // Copiar tradução (cria aqui, mas começa escondido)
    copyTrBtn_ = new QPushButton("TR", buttonBar_);
    copyTrBtn_->setFixedSize(40, 24);
    copyTrBtn_->setCursor(Qt::PointingHandCursor);
    layout->addWidget(copyTrBtn_);
    copyTrBtn_->hide();

    QObject::connect(closeBtn, &QPushButton::clicked, this, [this]() {
        if (buttonBar_)
            buttonBar_->close();
        this->close();
    });

    QObject::connect(copyOcrBtn_, &QPushButton::clicked, this, [this]() {
        QClipboard* cb = QGuiApplication::clipboard();
        cb->setText(ocrText_);
    });

    QObject::connect(copyTrBtn_, &QPushButton::clicked, this, [this]() {
        QClipboard* cb = QGuiApplication::clipboard();
        cb->setText(translatedText_.isEmpty() ? translatedText_ : translatedText_);
    });

    buttonBar_->adjustSize();
    updateButtonBarPosition();
    buttonBar_->show();
}


void FloatingPane::setOverlayText(const QString& text) {
    translatedText_ = text;
    if (copyTrBtn_) {
        copyTrBtn_->setVisible(!translatedText_.isEmpty());
    }
    update();
}


void FloatingPane::setRawText(const QString& text) {
    ocrText_ = text;
    if (copyOcrBtn_) {
        copyOcrBtn_->setVisible(!ocrText_.isEmpty());
    }
    update();
}


void FloatingPane::paintEvent(QPaintEvent*) {
    QPainter p(this);

    p.drawPixmap(0, 0, width(), height(), img_);

    if (translatedText_.isEmpty())
        return;

    p.setPen(Qt::white);
    p.setRenderHint(QPainter::TextAntialiasing, true);

    const int margin = 8;
    QRect maxRect = rect().adjusted(margin, margin, -margin, -margin);

    QRect textRect = p.boundingRect(
        maxRect,
        Qt::TextWordWrap | Qt::AlignLeft | Qt::AlignTop,
        translatedText_
        );

    textRect = textRect.intersected(maxRect);
    QRect bgRect = textRect.adjusted(-4, -4, 4, 4);
    p.fillRect(bgRect, QColor(0, 0, 0, 140));

    // desenha o texto dentro do textRect
    QTextOption opt;
    opt.setWrapMode(QTextOption::WordWrap);
    opt.setAlignment(Qt::AlignLeft | Qt::AlignTop);
    p.drawText(textRect, translatedText_, opt);
}


void FloatingPane::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        dragStartPosGlobal_ = event->globalPosition().toPoint();
#else
        dragStartPosGlobal_ = event->globalPos();
#endif
        windowStartPos_ = frameGeometry().topLeft();
    }
    QWidget::mousePressEvent(event);
}


void FloatingPane::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() & Qt::LeftButton) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QPoint currentGlobal = event->globalPosition().toPoint();
#else
        QPoint currentGlobal = event->globalPos();
#endif
        QPoint delta = currentGlobal - dragStartPosGlobal_;
        move(windowStartPos_ + delta);
    }
    QWidget::mouseMoveEvent(event);
}


void FloatingPane::moveEvent(QMoveEvent* event) {
    QWidget::moveEvent(event);
    updateButtonBarPosition();
}


void FloatingPane::updateButtonBarPosition() {
    if (!buttonBar_)
        return;

    // tela onde a janela está
    QScreen* scr = QGuiApplication::screenAt(frameGeometry().center());
    if (!scr)
        scr = QGuiApplication::primaryScreen();

    QRect avail = scr->availableGeometry();

    const int margin = 6;
    QSize barSize = buttonBar_->size();
    QRect paneRect = frameGeometry();

    int x = paneRect.right() + margin;
    int y = paneRect.top();

    if (x + barSize.width() > avail.right()) {
        x = paneRect.right() - barSize.width();
        y = paneRect.top() - barSize.height() - margin;
    }

    if (x < avail.left())
        x = avail.left();
    if (y < avail.top())
        y = avail.top();
    if (y + barSize.height() > avail.bottom())
        y = avail.bottom() - barSize.height();

    buttonBar_->move(x, y);
}

