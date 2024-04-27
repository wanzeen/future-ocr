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
    void signalGraphicsPixmapItemAdded();

public:
    CustomGraphicsView(QWidget *parent=nullptr);
    void resetMouseState();
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    // 用于指定在拖放进入窗口时的行为
    void dragEnterEvent(QDragEnterEvent *event) override;

    // 此处必须要有调用，不然不会触发 dropEvent事件
    void dragMoveEvent(QDragMoveEvent *event) override;

    // 用于指定在拖放释放时的行为
    void dropEvent(QDropEvent *event) override ;

    // 快捷键
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;


};

#endif // CUSTOMGRAPHICSVIEW_H
