#ifndef IMAGETOOL_H
#define IMAGETOOL_H
#include <QImage>
#include <QScrollArea>
#include <QMainWindow>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/opencv.hpp>
#include <leptonica/allheaders.h>
#include <QLabel>

class ImageWindow:public QMainWindow
{
private:
    QLabel *label;
    QScrollArea *scrollArea;
public:
    ImageWindow(QWidget *parent=nullptr);
    void displayImage(QImage image,QString title);
    ~ImageWindow();
};

class ImageTool {
public:
    static void showImage(QImage image,QString title);
    // 图片格式转换： QImage 转为 PIX
    static PIX* qImageToPix(QImage &qImage);

};

#endif // IMAGETOOL_H
