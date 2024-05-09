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
signals:
    //图片选中
    void signalGraphicsViewSelected(QPixmap selectedPixmap);
    // 图像添加消息事件
    void signalGraphicsPixmapItemAdded();
    // 图像改变事件
    void signalGraphicsPixmapChangedDescription(QString description);
public:
    CustomGraphicsView(QWidget *parent=nullptr);
    // 鼠标状态重置
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
    // 滚轮滚动
    void wheelEvent(QWheelEvent *event) override;
    // 快捷键
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    // 图像旋转
    QImage getRotatedImage(QImage &originImage,int degree);
    void updatePixmap(QPixmap &pixmap);
    // 设置源图像
    void setSrcImage(QImage &image);
    // 获取当前图像
    QPixmap getCurrPixmap();

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
    qreal angleValue = 0.0;
    qreal scaleValue = 1.0;
    QGraphicsScene *imageScene;
    // 原始图片
    QImage srcImage;
};

#endif // CUSTOMGRAPHICSVIEW_H
