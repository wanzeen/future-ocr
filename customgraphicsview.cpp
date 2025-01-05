#include "customgraphicsview.h"
#include <QGraphicsPixmapItem>
#include <QFileInfo>
#include <QMouseEvent>
#include <QMimeData>
#include <QGraphicsScene>
#include "imagetool.h"
CustomGraphicsView::CustomGraphicsView(QWidget *parent):QGraphicsView(parent) {
    qDebug()<<"初始化CustomGraphicsView.";
    setAcceptDrops(true);
    imageScene = new QGraphicsScene();
    setScene(imageScene);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}



void CustomGraphicsView::resetMouseState(){
    this->currentMouseState = MouseState::Init;
    scenePosStart = QPointF(0,0);
    scenePosEnd = QPointF(0,0);
    selectionRect = QRectF();

}
void CustomGraphicsView::mousePressEvent(QMouseEvent *event) {
    // qDebug()<<"mousePressEvent...";
    if(this->imageMode==1){
        return;
    }
    if(isDrawBoxes && !bakPixmap.isNull()){
        // 清除选区
        this->updatePixmap(bakPixmap);
        isDrawBoxes = false;
    }
    if(event->button() != Qt::LeftButton){
        return;
    }
    // startPoint = event->pos();
    // 转换鼠标按下的位置到场景坐标系
    // qDebug()<<"press startPoint:"<<startPoint;

    scenePosStart = this->mapToScene(event->pos());
    scenePosEnd = scenePosStart;
    //qDebug()<<"press scenePos:"<<scenePos;

    //endPoint = event->pos();
    currentMouseState = MouseState::Press;
    this->viewport()->update();
}
void CustomGraphicsView::mouseMoveEvent(QMouseEvent* event){
    // qDebug()<<"mouseMoveEvent...";
    if(this->imageMode==1){
        return;
    }
    if (window()->isActiveWindow()) {
        QGraphicsView::mouseMoveEvent(event);
    } else {
        window()->activateWindow();
    }
    // 更新选定的区域
    if (currentMouseState == MouseState::Press || currentMouseState == MouseState::Drag)
    {
        currentMouseState = MouseState::Drag;
        // endPoint = event->pos();
        scenePosEnd = this->mapToScene(event->pos());
        this->viewport()->update();
    }
}
void CustomGraphicsView::mouseReleaseEvent(QMouseEvent *event) {
    // qDebug()<<"mouseRelease...";
    //不是鼠标左键
    if(this->imageMode==1){
        return;
    }
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

    // endPoint = event->pos();
    scenePosEnd = this->mapToScene(event->pos());
    // qDebug()<<"release endPoint:"<<endPoint;
    // qDebug()<<"release scenePosEnd:"<<scenePosEnd;

    if(!this->scene()->items().isEmpty()){
        //刷新选区，消除选中样式
        currentMouseState = MouseState::Refresh;
        this->viewport()->update();
        // 合并两个坐标为一个矩形选框
        // QRect selectedRect(startPoint, endPoint);

        // 计算矩形区域
        selectionRect = QRectF(scenePosStart, scenePosEnd).normalized();

        //截图选区图片
        // QPixmap selectedPixmap = this->grab(selectedRect);

        // 也可以是QImage
        //QImage selectedImage(selectionRect.size().toSize(), QImage::Format_ARGB32);
        // 选中区域图像大小
        QPixmap  selectedImage(selectionRect.size().toSize());
        // QImage fullImage(this->sceneRect().size().toSize(), QImage::Format_ARGB32);
        selectedImage.fill(Qt::transparent);
        QPainter painter(&selectedImage);
        // 设置抗锯齿渲染，减少图像边缘锯齿效果的技术，使得图像边缘更加平滑和清晰
        painter.setRenderHint(QPainter::Antialiasing);
        // 将selectedRect矩形在场景坐标系中进行平移，使其位置与场景的左上角对齐，然后将结果保存在sceneRect中
        QRectF sceneRect = selectionRect.translated(imageScene->sceneRect().topLeft());
        // 将场景中的选中区域渲染到图像中时，只渲染sceneRect所指定的部分
        imageScene->render(&painter, QRectF(), sceneRect);
        painter.end();
        // 显示图像
        // ImageTool::showImage(selectedImage.toImage(),"选中预览");
        // testTransform(selectedPixmap);
        // 发送OCR识别
        emit signalGraphicsViewSelected(selectedImage);
        // 重新刷新选区为选中状态
        currentMouseState = MouseState::Selected;
        this->viewport()->update();
    }
}

// 常见颜色：
// QColor(0,0,255) 标准蓝色
// QColor(255, 0, 0) 红色

void CustomGraphicsView::paintEvent(QPaintEvent *event){
    // qDebug()<<"View Paint Event.";
    QGraphicsView::paintEvent(event);
    //开始绘制，以当前窗口作为绘制设备
    // QPainter painter(this->viewport());
    QPainter painter(this->viewport());
    // 绘制选区
    if(this->scene()->items().isEmpty()){
        // qDebug()<<"items is empty";
        //painter.setPen(QPen(QColor(0, 180, 255,128), 2));
        // 创建一个QPen对象并设置颜色为浅黑色
        QPen pen(Qt::darkGray);
        painter.setPen(pen);
        painter.drawText(event->rect(), Qt::AlignCenter, "拖放或【Ctr+V】粘贴图片");
    }

    // qDebug()<<"painting w="<<this->width()<<", h:"<<this->height();
    // qDebug()<<"currentMouseState:"<<currentMouseState;
    if(currentMouseState==MouseState::Init || currentMouseState == MouseState::Refresh || this->scene()->items().isEmpty()){
        // qDebug()<<"paintEvent return.";
        painter.end();
        return;
    }

    QPointF topLeft = this->mapFromScene(scenePosStart);
    QPointF bottomRight = this->mapFromScene(scenePosEnd);
    QRectF sceneRect(topLeft, bottomRight);
    // 获取矩形边界和视口部分的交集，以确保获取整个图像区域
    QRectF selectedRect = sceneRect.intersected(this->viewport()->rect());
    //QRect selectedRect(startPoint, endPoint);
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

    // QRectF sceneRect = this->sceneRect();
    // painter.fillRect(selectedRect, overlayColor);
    // QRectF sceneRect = selectionRect.boundingRect();

    // this->mapToScene(selectionRect).boundingRect();
    // 矩形区域，填充颜色，叠加模式
    painter.fillRect(selectedRect, overlayColor);

    // 添加设置合成模式为覆盖模式
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    // 矩形边框线绘制
    // QPen(QColor(0, 0, 255), 1)
    painter.setPen( QPen(QColor(0, 180, 255), 1.5));
    //给选中的图片画上蓝色边框
    // painter.drawRect(selectedRect);
    painter.drawRect(selectedRect);


}

void CustomGraphicsView::dragEnterEvent(QDragEnterEvent *event) {
    // qDebug()<<"dragEnterEvent...";
    if (event->mimeData()->hasUrls()) {
            // 接受拖放操作
            event->acceptProposedAction();
            //event->accept();
    }else{
        event->ignore();
    }

}
void CustomGraphicsView::dragMoveEvent(QDragMoveEvent *event) {
    // qDebug()<<"dragMoveEvent...";
    event->accept();
}

// 用于指定在拖放释放时的行为
void CustomGraphicsView::dropEvent(QDropEvent *event) {
    qDebug()<<"监听到拖放事件.";
    if (event->mimeData()->hasUrls()) {
        QList<QUrl> urlList = event->mimeData()->urls();
        QString imagePath = urlList.first().toLocalFile();
        qDebug()<<"获取文件路径:"<<imagePath;
        QFileInfo fileInfo(imagePath);
        QString fileExtension = fileInfo.suffix().toLower();
        if (fileExtension == "jpg" || fileExtension == "jpeg" || fileExtension == "png" || fileExtension == "gif") {
            // 是图片文件
            // 加载并展示图片
            QPixmap pixmap(imagePath);

            if (!pixmap.isNull()) {
                // if(!this->scene()->items().isEmpty()){
                //     this->scene()->removeItem(this->scene()->items().first());
                //     // this->resetMouseState();
                // }
                // bakPixmap = pixmap;
                // QGraphicsPixmapItem *item = new QGraphicsPixmapItem(pixmap);
                this->setProperty("fileName",fileInfo.fileName());
                // this->scene()->addItem(item);
                QImage image = pixmap.toImage();
                this->setSrcImage(image);
                this->updatePixmap(pixmap);
                emit signalGraphicsPixmapItemAdded();
                qDebug()<<"开始识别拖放所得的图片.";
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
void CustomGraphicsView::setSrcImage(QImage &image){
    this->srcImage = image;
    this->bakPixmap = QPixmap();
    // this->isDrawBoxes = false;
}


void CustomGraphicsView::wheelEvent(QWheelEvent *event)
{
    // qDebug()<<"滚动轮...";
    // 检查是否按下了 Ctrl + 滚轮
    // 图像放大、缩小监听
    if(event->modifiers()== Qt::ControlModifier && event->angleDelta().y() != 0 && !(event->modifiers() & Qt::ShiftModifier)){
        // qDebug()<<"ctr+滚轮.";
        // 向前滚动，放大
        if(!this->items().isEmpty()){
            qreal offset = 0;
            if (event->angleDelta().y() > 0) {
                offset = 0.1;
                // scale += 0.2;
            } else if(event->angleDelta().y()<0) {
                offset = -0.1;
                // scale -= 0.2;
            }
            qreal currScale = scaleValue + offset;
            if((currScale<0.1) || currScale>5.0){
                emit signalGraphicsPixmapChangedDescription("缩放已达到极限："+QString::number(qRound(scaleValue*100))+"%");
                event->accept();
                return;
            }
            this->scaleValue = currScale;
            QImage currImage = getRotatedImage(this->srcImage,this->angleValue);


            // 不要在已有图片图片的基础上缩放，多次缩放会导致图片越来越模糊，每次缩放应该在原图的基础上进行缩放
            // QGraphicsPixmapItem *item = static_cast<QGraphicsPixmapItem *>(scene->items().first());
            // QImage currImage = item->pixmap().toImage();
            // QSize currSize = image.size();
            // image.scaled(newSize,)

            QSize newSize(currImage.width()*currScale,currImage.height()*currScale);
            QImage targetImage = currImage.scaled(newSize,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
            QPixmap pixmap = QPixmap::fromImage(targetImage);
            this->updatePixmap(pixmap);
            QString message = QString("缩放大小：%1% 分辨率：%2 x %3").arg(qRound(scaleValue*100)).arg(pixmap.width()).arg(pixmap.height());
            emit signalGraphicsPixmapChangedDescription(message);

        }
        // qDebug()<<"scaleValue:"<<this->scaleValue;
        // 锁定视图以防止并发修改
        // view->blockSignals(true);
        // view->scale(scale, scale);
        // view->blockSignals(false);
        event->accept();

    } else if( event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier) && event->angleDelta().y() != 0)
    {
        // qDebug()<<"ctr+shift+滚轮.";
        if(!imageScene->items().isEmpty()){
            // 忽略滚轮事件，防止滚动条滚动
            //event->ignore();
            // 开始旋转
            if(event->angleDelta().y() > 0){
                this->angleValue -= 1;
            }else if(event->angleDelta().y()<0) {
                this->angleValue += 1;
            }
            QImage targetImage;
            if(this->angleValue!=0){
                targetImage = this->getRotatedImage(this->srcImage,this->angleValue);
            }else{
                targetImage = this->srcImage;
            }
            QSize newSize(targetImage.width()*this->scaleValue,targetImage.height()*this->scaleValue);
            targetImage = targetImage.scaled(newSize,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
            QPixmap newPixmap = QPixmap::fromImage(targetImage);
            // scene->items()
            // items.at(0) =
            // item->setPixmap(newPixmap);
            // QGraphicsPixmapItem *newItem = new QGraphicsPixmapItem(newPixmap);
            this->updatePixmap(newPixmap);
            //noteLabel->setText("旋转角度："+QString::number(fmod(angle, 360)));
            emit signalGraphicsPixmapChangedDescription("旋转角度："+QString::number(fmod(angleValue, 360))+"°");
           // QRectF rect = targetImage.rect().toRectF();
            //重新调整大小
            //this->setSceneRect(rect);
            //需要更新，不然显示不全
            //imageScene->update();
            event->accept();
        }
    }else{
        // qDebug()<<"call parent wheelEvent";
        QGraphicsView::wheelEvent(event);
    }
}

QImage CustomGraphicsView::getRotatedImage(QImage &originImage,int degree){
    // 将原点移动到中心
    QTransform transform;
    // transform = transform.translate(originImage.width() / 2, originImage.height() / 2);
    transform = transform.rotate(degree);
    // 旋转后的矩形大小
    QRectF rotatedRect = transform.mapRect(originImage.rect());
    QSize rotatedSize = rotatedRect.size().toSize();
    // qDebug()<<"rotateSize:"<<rotatedSize;
    // 创建一个QPainter对象，并将其关联到当前窗口
    //图像旋转后，大小会发生改变
    QImage targetImage = originImage.transformed(transform,Qt::SmoothTransformation);
    //qDebug()<<"targetImage size:"<<targetImage.size();
    QPainter painter;
    // 绘制之前，要确保图片具有一定大小，不然会报错
    QImage image(rotatedSize, this->srcImage.format());
    painter.begin(&image);
    // 填充白色
    painter.fillRect(image.rect(), Qt::white);
    // 绘制新的内容
    painter.setPen(Qt::black);
    //painter.setFont(QFont("Arial", 20));
    // painter.drawText(image.rect(), Qt::AlignCenter, "Dynamic Image");
    painter.drawImage(0, 0, targetImage);
    painter.end();
    return image;
    // return targetImage;
}


QPixmap CustomGraphicsView::getCurrPixmap(){
    if(!imageScene->items().isEmpty()){
        // 打开文件选择器
       QGraphicsPixmapItem *item = static_cast<QGraphicsPixmapItem *>(imageScene->items().first());
       return item->pixmap();
    }
    return QPixmap();
}
void CustomGraphicsView::updatePixmap(QPixmap &pixmap){
    // 创建一个新的 QGraphicsPixmapItem 并将其添加到场景中
    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(pixmap);
    if(!this->items().isEmpty()){
        QGraphicsItem *oldImage = this->items().first();
        this->imageScene->removeItem(oldImage);
        delete oldImage;
    }
    this->resetMouseState();
    // this->srcImage = pixmap.toImage();
    // this->bakPixmap = pixmap;
    imageScene->addItem(item);
    QRectF rect = pixmap.rect().toRectF();
    //重新调整大小
    this->setSceneRect(rect);
    imageScene->update();
    // 创建视图
    // 将场景设置到视图中
    // this->setScene(scene);
    // 设置缩放比例
    //view->scale(0.5, 0.5);
    // 将项目居中显示
    // item->setPos((scene->width() - item->boundingRect().width()) / 2,
    //              (scene->height() - item->boundingRect().height()) / 2);
}

void CustomGraphicsView::receiveOcrRegionSelected(QVector<QVector<QPointF>> *boxes){
    qDebug()<<"更新被识别区域.";
    this->boxes = boxes;
    isDrawBoxes = true;
    QPixmap currPixmap = this->getCurrPixmap();
    bakPixmap = currPixmap;
    QPainter painter(&currPixmap);
    if(isDrawBoxes){
        // QColor lightBlueColor(173, 216, 230);
        QPointF topLeftPoint = selectionRect.topLeft();
        painter.setPen(QPen(QColor(0, 180, 255), 1.2));
        // painter.setPen(QPen(QColor(0, 0, 255), 1));
            // (QPen(QPen(QColor(0, 0, 255), 1)));
        // qDebug()<<"boxes.size:"<<boxes->size();

        // 释放每个一维向量的内存
        for (int i = 0; i < boxes->size(); ++i) {
            QVector<QPointF>& regions = (*boxes)[i];
            for(int k=0;k<regions.size();k++){
                regions[k] += topLeftPoint;
            }
            painter.drawPolygon(regions,Qt::OddEvenFill);
            regions.clear(); // 清空一维向量中的元素
        }
        // 清空二维向量数组
        boxes->clear();
        // 释放二维向量数组本身的内存
        delete boxes;
        // for(QVector<QPointF> regions:*boxes){
        //     for(int k=0;k<regions.size();k++){
        //         regions[k] += topLeftPoint;
        //     }
        //     painter.drawPolygon(regions,Qt::OddEvenFill);
        //     //qDebug()<<"pllygon..";
        // }
        // qDebug()<<"选区绘制完成.";
        painter.end();
        this->updatePixmap(currPixmap);
        // ImageTool::showImage(currPixmap.toImage(),"绘制结果");
        // isDrawBoxes = false;
    }
    // imageScene->update();
}
void CustomGraphicsView::setImageMode(int value){
    imageMode = value;
}

