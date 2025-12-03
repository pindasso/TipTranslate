#pragma once
#include <QWidget>
#include <QRect>

class OverlaySelect : public QWidget {
    Q_OBJECT
public:
    explicit OverlaySelect(QWidget* parent=nullptr);
signals:
    void areaSelected(const QRect& rect, const QPixmap& grab);
protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void keyPressEvent(QKeyEvent*) override;
private:
    QRect selection_;
    QPoint origin_;
    bool dragging_ = false;
};
