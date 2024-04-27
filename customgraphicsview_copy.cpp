#include "customgraphicsview_copy.h"
#include <QGraphicsPixmapItem>
CustomGraphicsView::CustomGraphicsView(QWidget *parent):QGraphicsView(parent) {
}

void CustomGraphicsView::mousePressEvent(QMouseEvent *event) {
    if(event->button() != Qt::LeftButton){
        return;
    }
    startPoint = event->pos();
    endPoint = event->pos();
    currentMouseState = MouseState::Press;
    this->update();
}
void CustomGraphicsView::mouseMoveEvent(QMouseEvent* event){

    // if(lastRect.contains(event->pos())){
    //     this->setCursor(Qt::PointingHandCursor);
    // }else{
    //     this->setCursor(Qt::ArrowCursor);
    // }

    // 更新选定的区域
    if (currentMouseState == MouseState::Press || currentMouseState == MouseState::Drag)
    {
        // QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        //this->setCursor(Qt::ArrowCursor);
        currentMouseState = MouseState::Drag;
        endPoint = event->pos();
        this->update();
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
        this->update();
        return;
    }
    //选中
    currentMouseState = MouseState::Selected;
    //QRect rect(startPoint,endPoint);
    // 合并两个坐标为一个矩形选框
    QRect selectedRect(startPoint, endPoint);
    QPixmap selectedPixmap = grab(selectedRect);
    qDebug()<<"已选中";
    this->update();
}
// void CustomGraphicsView::mouseDoubleClickEvent(QMouseEvent *event){

// }

  // bool CustomGraphicsView::eventFilter(QObject *watched, QEvent *event) {
  //       if (watched == this->la && event->type() == QEvent::Paint) {

  //        }
  // }

void CustomGraphicsView::paintEvent(QPaintEvent *event){


    // QGraphicsPixmapItem *item = static_cast<QGraphicsPixmapItem *>(this->scene()->items().first());
    // QPixmap pixmap = item->pixmap();

    // if(!scene->items().isEmpty()){
    //     scene->removeItem(scene->items().first());
    // }
    //scene->removeItem();
    //scene->addItem(item);

    // 创建视图
    // 将场景设置到视图中
    // view->setScene(scene);
    //item->setPixmap(pixmap);

    //开始绘制，以当前窗口作为绘制设备
    qDebug()<<"painting w="<<this->width()<<", h:"<<this->height();
    if(!this->scene()->items().isEmpty()){
        qDebug()<<"内容不为空";
        QPainter painter(this);
        QRect selectedRect(startPoint, endPoint);
        QPixmap selectedPixmap = grab(selectedRect);
        painter.fillRect(selectedRect, QColor(0, 180, 255));
    }
    QGraphicsView::paintEvent(event);
    //QGraphicsView *view = this->scene()->views().first();
    // painter.begin(this);
    // //设置阴影
    // QColor shadowColor = QColor(0, 0, 0, 120);
    // //绘制抓取的整个图片
    // painter.drawPixmap(0, 0, windowPixmap);
    // //绘制上阴影
    // painter.fillRect(windowPixmap.rect(), shadowColor);

    // 调试输出
        // painter.setPen(QPen(QColor(0, 180, 255), 2));
        // painter.drawText(lastRect, Qt::AlignCenter, "正在识别选区文字...");
        // painter.end();
        // isDoubleClick = false;
        // return;

    // 合并两个坐标为一个矩形选框
    // QRect selectedRect(startPoint, endPoint);
    //QPixmap selectedPixmap = grab(selectedRect);
    // painter.fillRect(selectedRect, QColor(0, 180, 255));

    //绘制选中的图片
    //painter.drawPixmap(selectedRect.topLeft(), capturedPixmap);

    //设置画笔
    //painter.setPen(QPen(Qt::red, 2));
    //painter.setPen(QPen(QColor(0, 180, 255), 2));
    //给选中的图片画上蓝色边框
    //painter.drawRect(rect);
    // if(currentOperate==OperateState::Init){
    //     painter.drawText(event->rect(), Qt::AlignCenter, "提示：按下【鼠标左键】并拖动开始截图.");
    // }else if(currentOperate==OperateState::Finish){
    //     painter.drawText(rect, Qt::AlignCenter, "提示：按下【Enter键】或【鼠标左键双击】识别选区文字.");
    //     lastRect = rect;
    // }else if(currentOperate==OperateState::ClickNoDrag){
    //     painter.drawText(event->rect(), Qt::AlignCenter, "提示：未选中截图.");
    // }
    // painter.end();
}


