#include "customgraphicsview.h"
#include <QGraphicsPixmapItem>
CustomGraphicsView::CustomGraphicsView(QWidget *parent):QGraphicsView(parent) {
}
void CustomGraphicsView::resetMouseState(){
    this->currentMouseState = MouseState::Init;
}
void CustomGraphicsView::mousePressEvent(QMouseEvent *event) {
    if(event->button() != Qt::LeftButton){
        return;
    }
    startPoint = event->pos();
    endPoint = event->pos();
    currentMouseState = MouseState::Press;
    this->viewport()->update();
}
void CustomGraphicsView::mouseMoveEvent(QMouseEvent* event){
    // 更新选定的区域
    if (currentMouseState == MouseState::Press || currentMouseState == MouseState::Drag)
    {
        currentMouseState = MouseState::Drag;
        endPoint = event->pos();
        this->viewport()->update();
    }
}
void CustomGraphicsView::mouseReleaseEvent(QMouseEvent *event) {
    //不是鼠标左键
    if(event->button() != Qt::LeftButton){
        return;
    }
    //只是点击事件
    if(currentMouseState != MouseState::Drag){
        currentMouseState = MouseState::PressNoDrag;
        this->viewport()->update();
        return;
    }
    //选中
    currentMouseState = MouseState::Selected;

    if(!this->scene()->items().isEmpty()){
        //刷新选区，消除选中样式
        currentMouseState = MouseState::Refresh;
        this->viewport()->update();
        // 合并两个坐标为一个矩形选框
        QRect selectedRect(startPoint, endPoint);
        //截图选区图片
        QPixmap selectedPixmap = grab(selectedRect);
        // 发送OCR识别
        emit signalGraphicsViewSelected(selectedPixmap);
        // 重新刷新选区为选中状态
        currentMouseState = MouseState::Selected;
        this->viewport()->update();
    }
}

// 常见颜色：
// QColor(0,0,255) 标准蓝色
// QColor(255, 0, 0) 红色

void CustomGraphicsView::paintEvent(QPaintEvent *event){
    QGraphicsView::paintEvent(event);
    //开始绘制，以当前窗口作为绘制设备
    //qDebug()<<"painting w="<<this->width()<<", h:"<<this->height();
    if(currentMouseState==MouseState::Init || currentMouseState == MouseState::Refresh || this->scene()->items().isEmpty()){
        return;
    }
    QPainter painter(this->viewport());
    QRect selectedRect(startPoint, endPoint);
    //qDebug()<<"selected area  w:"<<selectedRect.width()<<", h:"<<selectedRect.height();
    if(selectedRect.height()<=1 || selectedRect.width()<= 1){
        return;
    }
    // 设置要添加的颜色及其透明度
    QColor overlayColor(0, 180, 255,128);
    // 在目标图像上绘制选中效果
    // 设置合成模式为叠加模式
    painter.setCompositionMode(QPainter::CompositionMode_Overlay);
    // 色填充整个目标图像
    painter.fillRect(selectedRect, overlayColor);
        //设置合成模式为覆盖模式
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    // 绘制选区边框
    painter.setPen(QPen(QColor(0, 0, 255), 1));
    //给选中的图片画上蓝色边框
    painter.drawRect(selectedRect);

}


