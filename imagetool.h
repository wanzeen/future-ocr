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
    static QImage matToQImage(cv::Mat mat);
    static PIX* matToPix(cv::Mat &imgGray);
    static PIX* qImageToPix(QImage &qImage);
    static QImage toGrayBinaryImage(QImage &qImage);
    static cv::Mat pixToMat(PIX* pix);


};

#endif // IMAGETOOL_H
