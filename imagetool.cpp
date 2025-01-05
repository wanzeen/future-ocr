#include "imagetool.h"

ImageWindow::ImageWindow(QWidget *parent): QMainWindow(parent){
    // 创建 QLabel 对象
    label = new QLabel();
    // 创建 QScrollArea 对象
    scrollArea = new QScrollArea();
    scrollArea->setWidget(label);
    this->setCentralWidget(scrollArea);
}
void ImageWindow::displayImage(QImage image,QString title){
    // 将图像设置为 QLabel 的图像
    this->setWindowTitle(title);
    label->setPixmap(QPixmap::fromImage(image));
    // 调整QScrollArea的大小以适应图像
    qDebug()<<"image width:"<<image.width()<<", height:"<<image.height();
    label->resize(image.width(), image.height());
    this->show();
}

ImageWindow::~ImageWindow(){
    delete label;
    delete scrollArea;
    qDebug()<<"delete ImageTool.";
}



static Pix *toPix(QImage &qImage){
    Pix *pix = pixCreate(qImage.width(), qImage.height(), 32);
    for (int y = 0; y < qImage.height(); ++y) {
        for (int x = 0; x < qImage.width(); ++x) {
            QColor color = qImage.pixel(x, y);
            pixSetPixel(pix, x, y, color.rgb());
        }
    }
    return pix;
}


// static l_uint32 pixMakeRGB(l_uint32 rval, l_uint32 gval, l_uint32 bval) {
//     return (rval << 24) | (gval << 16) | (bval << 8);
// }




PIX* ImageTool::qImageToPix(QImage &qImage){
    PIX *pix = pixCreate(qImage.width(), qImage.height(), 32);
    for (int y = 0; y < qImage.height(); ++y) {
        for (int x = 0; x < qImage.width(); ++x) {
            QColor color = qImage.pixel(x, y);
            pixSetPixel(pix, x, y, color.rgb());
        }
    }
    return pix;
}


void ImageTool::showImage(QImage image,QString title)
{
    ImageWindow *window = new ImageWindow();
    window->displayImage(image,title);
}

