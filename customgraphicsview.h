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
public slots:
    // 多个小区域选中，绘制矩形框
    void receiveOcrRegionSelected(QVector<QVector<QPointF>> *boxes);

signals:
    //图片选中事件： 鼠标选中图像局部区域，通知主窗口、OCR等处理
    void signalGraphicsViewSelected(QPixmap selectedPixmap);
    // 图像添加事件: 图像区域拖拽、粘贴图像触发, 通知主窗口、OCR等处理
    void signalGraphicsPixmapItemAdded();
    // 图像改变事件：图像缩放，通知主窗口
    void signalGraphicsPixmapChangedDescription(QString description);

public:
    CustomGraphicsView(QWidget *parent=nullptr);   
    // 监听鼠标按下
    void mousePressEvent(QMouseEvent *event) override;
    // 鼠标移动
    void mouseMoveEvent(QMouseEvent* event) override;
    // 鼠标释放
    void mouseReleaseEvent(QMouseEvent *event) override;
    // 用于指定在拖放进入窗口时的行为
    void dragEnterEvent(QDragEnterEvent *event) override;

    // 此处必须要有调用，不然不会触发 dropEvent事件
    void dragMoveEvent(QDragMoveEvent *event) override;

    // 用于指定在拖放释放时的行为
    void dropEvent(QDropEvent *event) override ;
    // 滚轮滚动
    void wheelEvent(QWheelEvent *event) override;
    // 绘制图像
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    // 图像旋转
    QImage getRotatedImage(QImage &originImage,int degree);
    // 调用场景：1. MainWindow 使用Ctr + V 粘贴图片
    //          2. MainWindow 打开图片
    //          3. 收到截图
    //          4. 当前画布收到拖拽图片
    void updatePixmap(QPixmap &pixmap);
    // void setNewPixmap(QPixmap &pixmap);
    // 设置源图像
    void setSrcImage(QImage &image);
    // 获取当前图像
    QPixmap getCurrPixmap();

    void setImageMode(int value);

private:
    // 鼠标状态重置
    void resetMouseState();
    enum MouseState{
        Init = 0,
        Press,
        Drag,
        PressNoDrag,
        Selected,
        Refresh
    };
    // QPoint startPoint;
    // QPoint endPoint;
    MouseState currentMouseState = MouseState::Init;
    qreal angleValue = 0.0;
    qreal scaleValue = 1.0;
    QGraphicsScene *imageScene;
    // 原始图片
    QImage srcImage;

    QPointF scenePosStart;
    QPointF scenePosEnd;
    QRectF selectionRect;
    // 如果是OCR绘制选区，在鼠标点击时清除选区
    bool isDrawBoxes = false;
    // ocr识别到区域
    QVector<QVector<QPointF>> *boxes;
    // ocr更新前备份当前图像
    QPixmap bakPixmap;
    // 图像模式：0 默认图像可选中编辑，1 图像锁定
    int imageMode = 0;

};

#endif // CUSTOMGRAPHICSVIEW_H
