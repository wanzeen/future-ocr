#include "customgraphicsview.h"
#include <QGraphicsPixmapItem>
#include <QFileInfo>
#include <QMouseEvent>
#include <QMimeData>
#include <QGraphicsScene>
#include "imagetool.h"
CustomGraphicsView::CustomGraphicsView(QWidget *parent):QGraphicsView(parent) {
    imageScene = new QGraphicsScene();
    this -> setScene(imageScene);
    this->setAcceptDrops(true);
}


static void testTransform(QPixmap originalImage)
{

    // 加载原始图像
    // QPixmap originalImage(":/path/to/your/image.png");
    qDebug()<<"pixmap w:"<<originalImage.size();
    // 定义透视变换的四个点
    // QPointF srcPoints[4] = { QPointF(0, 0), QPointF(originalImage.width(), 0), QPointF(originalImage.width(), originalImage.height()), QPointF(0, originalImage.height()) };
    // QPointF dstPoints[4] = { QPointF(0, 0), QPointF(originalImage.width(), 100), QPointF(originalImage.width(), originalImage.height() - 100), QPointF(0, originalImage.height()) };

    /*
    // 定义透视变换的四个点
    QPolygonF srcPoints;
    srcPoints << QPointF(0, 0) << QPointF(originalImage.width(), 0) << QPointF(originalImage.width(), originalImage.height()) << QPointF(0, originalImage.height());

    QPolygonF dstPoints;
    dstPoints << QPointF(0, 0) << QPointF(originalImage.width(), 100) << QPointF(originalImage.width(), originalImage.height() - 100) << QPointF(0, originalImage.height());


    // 创建透视变换
    QTransform perspectiveTransform;
    QTransform::quadToQuad(srcPoints, dstPoints,perspectiveTransform);

    // 透视变换后的图片
    QPixmap transformedImage(originalImage.size());
    transformedImage.fill(Qt::transparent);

    // 进行透视变换
    QPainter painter(&transformedImage);
    painter.setTransform(perspectiveTransform);
    painter.drawPixmap(0, 0, originalImage);

    // 保存透视变换后的图片
    // transformedImage.save("transformed_image.png");
    ImageTool::showImage(transformedImage.toImage(),"Transform Image");
*/
    // 定义倾斜角度
    qreal skewAngle = 30.0;

    // 创建透视变换矩阵
    QTransform perspectiveTransform;
    qreal tanSkewAngle = tan(qDegreesToRadians(skewAngle));
    perspectiveTransform.setMatrix(1, 0, tanSkewAngle, 0, 1, 0, 0, 0, 1);

    // 透视变换后的图片
    QPixmap transformedImage(originalImage.size());
    transformedImage.fill(Qt::transparent);

    // 进行透视变换
    QPainter painter(&transformedImage);
    painter.setTransform(perspectiveTransform);
    painter.drawPixmap(0, 0, originalImage);
    ImageTool::showImage(transformedImage.toImage(),"Transform Image");

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
        QPixmap selectedPixmap = this->grab(selectedRect);
        // testTransform(selectedPixmap);
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
                if(!this->scene()->items().isEmpty()){
                    this->scene()->removeItem(this->scene()->items().first());
                    this->resetMouseState();
                }
                QGraphicsPixmapItem *item = new QGraphicsPixmapItem(pixmap);
                this->setProperty("fileName",fileInfo.fileName());
                this->scene()->addItem(item);
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
        this->resetMouseState();
    }
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

