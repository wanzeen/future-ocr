#include "screencapture.h"

ScreenCapture::ScreenCapture(QWidget *parent):QWidget(parent) {
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->setWindowState(Qt::WindowActive | Qt::WindowFullScreen);
    this->setMouseTracking(true);
    initScreenPixmap();
}

void ScreenCapture::initScreenPixmap(){
    // 获取屏幕列表
    // QList<QScreen*> screens = QGuiApplication::screens();
    // 获取当前鼠标所在屏幕
    QScreen* currentScreen = QGuiApplication::screenAt(QCursor::pos());
    this->setGeometry(currentScreen->geometry());
    // qDebug()<<"currentScreen->geometry():"<<currentScreen->geometry().bottomRight().x();
    windowPixmap = currentScreen->grabWindow(0);
    // QList<QScreen*> screens =QGuiApplication::screens();
    currentOperate = OperateState::Init;
    // 获取图片宽度和高度
    int width = windowPixmap.width();
    int height = windowPixmap.height();
    // 计算中心点坐标
    QPoint centerPoint(width / 2, height / 2);
    startPoint = centerPoint;
    endPoint = centerPoint;

}

void ScreenCapture::setCapturePreference(bool isAutoOcr){
    this->isAutoOcr = isAutoOcr;
}


void ScreenCapture::mousePressEvent(QMouseEvent *event) {
    //qDebug()<<"press:"<<event->type()<<",btn:"<<event->button();
    if(event->button()!=Qt::LeftButton){
        return;
    }
    //上次也是点击
    currentOperate = OperateState::Press;
    // 开始记录鼠标移动事件
    // QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
    // 获取鼠标按下的位置
    startPoint = event->pos();
    endPoint = event->pos();
    update();
    //qDebug()<<"press";
    //eventHandler(event);
    //return QWidget::mousePressEvent(event);
}
void ScreenCapture::mouseMoveEvent(QMouseEvent* event){

    //qDebug()<<"move:"<<event->type()<<", btn:"<<event->button();
    // move: QEvent::MouseMove , btn: Qt::NoButton
    //eventHandler(event);
    //return QWidget::mouseMoveEvent(event);
    //qDebug()<<"mouse move... ";

    if(lastRect.contains(event->pos())){
        this->setCursor(Qt::PointingHandCursor);
    }else{
        this->setCursor(Qt::ArrowCursor);
    }

    // 更新选定的区域
    if (currentOperate == OperateState::Press || currentOperate == OperateState::Drag)
    {
        // QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        this->setCursor(Qt::ArrowCursor);
        currentOperate = OperateState::Drag;
        endPoint = event->pos();
        // qDebug()<<"end pos:"<<endPoint;
        update();
    }
}
void ScreenCapture::mouseReleaseEvent(QMouseEvent *event){
    //qDebug()<<"release:"<<event->type()<<", btn:"<<event->button();;
    //不是鼠标左键
    if(event->button() != Qt::LeftButton){
        return;
    }
    //只是点击事件
    if(currentOperate != OperateState::Drag){
        currentOperate = OperateState::ClickNoDrag;
        this->update();
        return;
    }
    currentOperate = OperateState::Finish;
    endPoint = event->pos();
    // qDebug()<<"release end pos:"<<endPoint;

    if(isAutoOcr){
        QRect rect(startPoint, endPoint);
        QPixmap capturedPixmap = windowPixmap.copy(rect);
        emit signalCaptureFinished(capturedPixmap);
       this->close();
    }else{
        this->update();
    }
}


void ScreenCapture::keyPressEvent(QKeyEvent *event)
{
    //Eeter键完成截图
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        // qDebug()<<"prss enter...";
        QPixmap capturedPixmap = windowPixmap.copy(lastRect);
        qDebug()<<"发送截屏大小："<<capturedPixmap.size();
        emit signalCaptureFinished(capturedPixmap);
        this->close();
    }
    //Esc键退出截图
    if (event->key() == Qt::Key_Escape)
    {
        emit signalCaptureCancel();
        this->close();
        qDebug()<<"已取消截屏.";
    }
}

void ScreenCapture::mouseDoubleClickEvent(QMouseEvent *event) {
    //qDebug()<<"doubleClick.";
    if(this->cursor().shape() == Qt::PointingHandCursor){
        isDoubleClick = true;
        QPixmap capturedPixmap = windowPixmap.copy(lastRect);
        emit signalCaptureFinished(capturedPixmap);
        this->close();
        qDebug()<<"双击鼠标发送的截屏大小："<<capturedPixmap.size();
    }else{
        isDoubleClick = false;
    }

}

void ScreenCapture::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    // QRect eventRect = event->rect();
    // qDebug()<<"cursor="<<this->cursor().pos();
    // qDebug()<<"painting w="<<this->width()<<", h:"<<this->height();

    //开始绘制，以当前窗口作为绘制设备
    painter.begin(this);
    //设置阴影
    QColor shadowColor = QColor(0, 0, 0, 120);
    //绘制抓取的整个图片
    painter.drawPixmap(0, 0, windowPixmap);
    //绘制上阴影
    painter.fillRect(windowPixmap.rect(), shadowColor);

    // 调试输出
    if(isDoubleClick){
        painter.setPen(QPen(QColor(0, 180, 255), 2));
        painter.drawText(lastRect, Qt::AlignCenter, "正在识别选区文字...");
        painter.end();
        isDoubleClick = false;
        return;
    }

    // 合并两个坐标为一个矩形选框
    QRect rect(startPoint, endPoint);
    //qDebug()<<"rect: w="<<rect.width()<<", h:"<<rect.width();
    //设置画笔
    //painter.setPen(QPen(Qt::red, 2));
    painter.setPen(QPen(QColor(0, 180, 255), 2));
    //获取选中的图片区域，不是一个点的时候开始截图
    if(rect.width()>1 && rect.height()>1){
        QPixmap capturedPixmap = windowPixmap.copy(rect);
        //绘制选中的图片
        painter.drawPixmap(rect.topLeft(), capturedPixmap);
        //给选中的图片画上蓝色边框
        painter.drawRect(rect);
    }
    if(currentOperate==OperateState::Init){
        painter.drawText(event->rect(), Qt::AlignCenter, "按住【鼠标左键】并拖动开始截图");
    }else if(currentOperate==OperateState::Finish){
        painter.drawText(rect, Qt::AlignCenter, "按下【Enter键】或【鼠标左键双击】识别选区文字");
        lastRect = rect;
    }else if(currentOperate==OperateState::ClickNoDrag){
        painter.drawText(event->rect(), Qt::AlignCenter, "未选中截图，请按住【鼠标左键】并拖动开始截图");
    }
    painter.end();

}


