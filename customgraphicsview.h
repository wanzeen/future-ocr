#ifndef CUSTOMGRAPHICSVIEW_H
#define CUSTOMGRAPHICSVIEW_H
#include <QGraphicsView>
#include <QMouseEvent>
#include <QPainter>
#include <QPoint>
// #include <QObject>

class CustomGraphicsView:public QGraphicsView
{
    Q_OBJECT
private:
    enum MouseState{
        Init = 0,
        Press,
        Drag,
        PressNoDrag,
        Selected,
        Refresh
    };
    QPoint startPoint;
    QPoint endPoint;
    MouseState currentMouseState = MouseState::Init;
signals:
    //图片选中
    void signalGraphicsViewSelected(QPixmap selectedPixmap);

public:
    CustomGraphicsView(QWidget *parent=nullptr);
    void resetMouseState();
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    // 快捷键
    void paintEvent(QPaintEvent *event) override;
};

#endif // CUSTOMGRAPHICSVIEW_H
