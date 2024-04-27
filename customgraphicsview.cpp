#include "customgraphicsview.h"
#include <QGraphicsPixmapItem>
#include <QFileInfo>
#include <QMouseEvent>
#include <QMimeData>

CustomGraphicsView::CustomGraphicsView(QWidget *parent):QGraphicsView(parent) {
    this->setAcceptDrops(true);
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
    // qDebug()<<"mouseRelease...";
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
    QPainter painter(this->viewport());
    if(this->scene()->items().isEmpty()){
        //qDebug()<<"is empty";
        //painter.setPen(QPen(QColor(0, 180, 255,128), 2));
        // 创建一个QPen对象并设置颜色为浅黑色
         QPen pen(Qt::darkGray);
        painter.setPen(pen);
        painter.drawText(event->rect(), Qt::AlignCenter, "拖放图片到此或【Ctr+V】粘贴图片");
    }
    //qDebug()<<"painting w="<<this->width()<<", h:"<<this->height();
    if(currentMouseState==MouseState::Init || currentMouseState == MouseState::Refresh || this->scene()->items().isEmpty()){
        return;
    }

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

void CustomGraphicsView::dragEnterEvent(QDragEnterEvent *event) {
    //qDebug()<<"dragEnterEvent...";
    if (event->mimeData()->hasUrls()) {
            // 接受拖放操作
            event->acceptProposedAction();
            //event->accept();
    }else{
        event->ignore();
    }

}
void CustomGraphicsView::dragMoveEvent(QDragMoveEvent *event) {
    event->accept();
}

// 用于指定在拖放释放时的行为
void CustomGraphicsView::dropEvent(QDropEvent *event) {
    // qDebug()<<"dropEvent.";
    if (event->mimeData()->hasUrls()) {
        QList<QUrl> urlList = event->mimeData()->urls();
        QString imagePath = urlList.first().toLocalFile();
        //qDebug()<<"imagePath:"<<imagePath;
        QFileInfo fileInfo(imagePath);
        QString fileExtension = fileInfo.suffix().toLower();
        if (fileExtension == "jpg" || fileExtension == "jpeg" || fileExtension == "png" || fileExtension == "gif") {
            // 是图片文件
            // 加载并展示图片
            QPixmap pixmap(imagePath);

            if (!pixmap.isNull()) {
                if(!this->scene()->items().isEmpty()){
                    this->scene()->removeItem(this->scene()->items().first());
                    this->resetMouseState();
                }
                QGraphicsPixmapItem *item = new QGraphicsPixmapItem(pixmap);
                this->setProperty("fileName",fileInfo.fileName());
                this->scene()->addItem(item);
                emit signalGraphicsPixmapItemAdded();
            }
        }
        // event->acceptProposedAction();
    }
}

void CustomGraphicsView::resizeEvent(QResizeEvent *event){
    //窗口大小变化事件
    // Q_UNUSED(event);
    // qDebug()<<"窗口大小变化..";
    // if(this->scene()->items().count()!=0){
    //     QGraphicsPixmapItem* item = dynamic_cast<QGraphicsPixmapItem*>(this->scene()->items().value(0));
    //     //图片适应窗口，会导致图片无法继续放大
    //     this->fitInView(item,Qt::KeepAspectRatio);
    // }
    QGraphicsView::resizeEvent(event);

}

