#ifndef CUSTOMGRAPHICSVIEW_COPY_H
#define CUSTOMGRAPHICSVIEW_COPY_H
#include <QGraphicsView>
#include <QMouseEvent>
#include <QPainter>
#include <QPoint>
class CustomGraphicsView:public QGraphicsView
{
    enum MouseState{
        Press,
        Drag,
        PressNoDrag,
        Selected
    };
private:
    QPoint startPoint;
    QPoint endPoint;
    MouseState currentMouseState;
public:
    CustomGraphicsView(QWidget *parent=nullptr);
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    // void mouseDoubleClickEvent(QMouseEvent *event) override;
    // 快捷键
    //void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    // bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // CUSTOMGRAPHICSVIEW_COPY_H
