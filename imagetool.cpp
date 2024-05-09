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


PIX* ImageTool::matToPix(cv::Mat &imgGray)
{
    int cols = imgGray.cols;
    int rows = imgGray.rows;

    qDebug()<<"depth:"<<imgGray.depth();
    PIX *pixS = pixCreate(cols, rows, 8);
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            pixSetPixel(pixS, j, i, (l_uint32)imgGray.at<uchar>(i, j));
        }
    }
    return pixS;
}

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
// static l_uint32 pixMakeRGB(l_uint32 rval, l_uint32 gval, l_uint32 bval) {
//     return (rval << 24) | (gval << 16) | (bval << 8);
// }
/*
Pix* toPix(cv::Mat &image){
    // 将 OpenCV Mat 数据复制到 Leptonica Pix 中

    // memcpy(pixGetData(pix), binaryImage.data, binaryImage.cols * binaryImage.rows * binaryImage.elemSize());

    //Pix* pix = pixReadMem(binaryImage.data, binaryImage.total());

    // 设置 Leptonica Pix 的格式

//     pixSetInputFormat(pix, IFF_RGB);
// }

// static Pix* matToPix(const cv::Mat& image) {
//     if (image.empty()) {
//         return nullptr;
//     }

    int width = image.cols;
    int height = image.rows;
    int depth = image.depth();
    int channels = image.channels();

    if (depth != CV_8U || channels != 1 && channels != 3) {
        return nullptr; // 仅支持8位单通道或3通道图像
    }

    Pix* pixImage = pixCreate(width, height, 8 * channels);
    if (!pixImage) {
        return nullptr; // 创建Pix对象失败
    }

    if (channels == 1) {
        // 单通道图像
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int pixelValue = image.at<uchar>(y, x);

                // pixSetPixel(pixImage, x, y, pixelValue);
            }
        }
    } else {
        // 3通道图像
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                cv::Vec3b pixel = image.at<cv::Vec3b>(y, x);
                l_uint32 pixelValue = pixMakeRGB(pixel[2], pixel[1], pixel[0]);
                // pixSetPixel(pixImage, x, y, pixelValue);
            }
        }
    }
    return pixImage;
}
*/
QImage ImageTool::matToQImage(cv::Mat mat){
    //cv::Mat mat;  // 假设这是您的OpenCV的Mat对象
    // 将cv::Mat转换为QImage
    // QImage image(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_RGB888);
    // // 如果需要，可以进行格式转换
    // QImage convertedImage = image.convertToFormat(QImage::Format_RGB32);
    QImage image;
    qDebug()<<"mat.type:"<<mat.type();
    switch(mat.type())
    {
    case CV_8UC1:
        // QImage构造：数据，宽度，高度，每行多少字节，存储结构
        image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
        break;
    case CV_8UC3:
        image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        image = image.rgbSwapped(); // BRG转为RGB
        // Qt5.14增加了Format_BGR888
        // image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.cols * 3, QImage::Format_BGR888);
        break;
    case CV_8UC4:
        image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        break;
    case CV_16UC4:
        image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGBA64);
        image = image.rgbSwapped(); // BRG转为RGB
        break;
    default:
        break;
    }
    return image;
    // return convertedImage;
}

// cv::Mat ImageTool::pixToMat(PIX* pix) {
//     int width = pixGetWidth(pix);
//     int height = pixGetHeight(pix);
//     int depth = pixGetDepth(pix);
//     int channels = depth / 8;  // Assuming 8 bits per channel
//     cv::Mat mat(height, width, CV_MAKETYPE(depth, channels));
//     memcpy(mat.data, pixGetData(pix), height * width * channels);
//     return mat.clone();  // Return a copy of the data
// }

// PIX可正确转为cv::Mat
cv::Mat ImageTool::pixToMat(PIX *pix){
    // 假设pix是一个PIX对象
    // 获取PIX对象的宽度和高度
    int width = pixGetWidth(pix);
    int height = pixGetHeight(pix);

    //CV_8UC4 表示一个深度为 32 的图像。这意味着每个像素由 4 个无符号 8 位整数（8UC4）表示，对应着蓝色、绿色、红色和透明度四个通道
    // 创建一个对应的OpenCV Mat对象
    cv::Mat image(cv::Size(width, height), CV_8UC4); // 假设PIX对象是RGBA格式，如果是其他格式，需要做相应调整

    // 将PIX对象的像素复制到cv::Mat对象中
    for (int y = 0; y < height; y++)
    {
        l_uint32* line = pixGetData(pix) + y * pixGetWpl(pix);
        for (int x = 0; x < width; x++)
        {
            l_uint32 pixel = line[x];
            int red = (pixel >> 24) & 0xFF;
            int green = (pixel >> 16) & 0xFF;
            int blue = (pixel >> 8) & 0xFF;
            int alpha = pixel & 0xFF;
            // 将像素信息存储到Mat对象中
            image.at<cv::Vec4b>(y, x) = cv::Vec4b(blue, green, red, alpha);
        }
    }
    return image;
}


void ImageTool::showImage(QImage image,QString title)
{
    ImageWindow *window = new ImageWindow();
    window->displayImage(image,title);
}

QImage ImageTool::toGrayBinaryImage(QImage &qImage){

    // 读取原始图像
    // const char *imgName = "D:/User/Qt/QtProject/images/mark.png";
    // cv::Mat image = cv::imread(imgName);
    // QImage qImage = pixmap.toImage();
    qDebug()<<"Oriainal Image:"<<qImage.width()<<","<<qImage.height();
    qDebug()<<"image depth:"<<qImage.depth();
    ImageTool::showImage(qImage,"Oriainal Image");

    QImage qtGrayImage = qImage.convertToFormat(QImage::Format_Grayscale8);
    qDebug()<<"grayImage.depth:"<<qtGrayImage.depth();
    ImageTool::showImage(qtGrayImage,"qtGrayImage Image");


    Pix *pix = toPix(qImage);
    cv::Mat image = pixToMat(pix);
    QImage img = matToQImage(image);
    showImage(img,"Pix to Image");
    // cv::Mat image = cv::Mat(qImage.height(), qImage.width(), CV_8UC4, const_cast<uchar*>(qImage.bits()), qImage.bytesPerLine());

    // cv::imshow("Origin Image", image);
    // 定义放大后的尺寸
    int newWidth = image.cols * 1.5;  // 原始宽度的两倍
    int newHeight = image.rows * 1.5;  // 原始高度的两倍
    // int newWidth = image.cols;
    // int newHeight = image.rows;
    // 使用cv::resize函数进行放大
    cv::Mat resizedImage;
    cv::resize(image, resizedImage, cv::Size(newWidth, newHeight));
    // cv::imshow("Resized Image", resizedImage);
    cv::Mat grayImage;
    // 灰度转换,使用cv::cvtColor函数将彩色图像转换为灰度图像
    ImageTool::showImage(matToQImage(resizedImage),"Resized Image");

    cv::cvtColor(resizedImage, grayImage, CV_BGR2GRAY);
    ImageTool::showImage(matToQImage(grayImage),"Gray Image");


    // 应用高斯滤波以减少噪声
    cv::Mat blurredImage;
    cv::GaussianBlur(grayImage, blurredImage, cv::Size(5, 5), 0);
    cv::Mat binaryImage;
    // 二值化可以将灰度图像转换为只包含黑白两种像素值的图像，以突出文字，可以使用适当的阈值方法来实现二值化
    cv::threshold(grayImage, binaryImage, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    // cv::Mat threshImage;
    // cv::adaptiveThreshold(blurredImage, threshImage, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 11, 2);
    qDebug()<<"binaryImage:"<<binaryImage.size().width<<","<<binaryImage.size().height;

    //QImage rgbImage(grayImage.data, grayImage.cols, grayImage.rows, static_cast<int>(grayImage.step), QImage::Format_RGB888);
    // ImageTool::showImage(matToQImage(binaryImage),"binaryImage");

    //灰度和二值化后的Mat，应该使用 QImage::Format_Grayscale8格式，表示图像的像素数据以8位灰度格式进行存储
    // 每个像素仅占用1个字节，表示灰度级别，范围从0（黑色）到255（白色）。这种格式适用于黑白图像或灰度图像，其中每个像素只有一个灰度值，没有彩色信息
    QImage grayScale8Image = QImage(binaryImage.data, binaryImage.cols, binaryImage.rows, binaryImage.step, QImage::Format_Grayscale8);
    // qImage = QImage(binaryImage.data, binaryImage.cols, binaryImage.rows, binaryImage.step, QImage::Format_RGB888);
    qDebug()<<"grayScale8Image:"<<grayScale8Image.size().width()<<","<<grayScale8Image.size().height();

    ImageTool::showImage(grayScale8Image,"GrayScale Image");

    // QImage::Format_RGB888 格式适合用于彩色图像处理，显然此处使用该方法不能正确显示
    //QImage colorImage = QImage(binaryImage.data, binaryImage.cols, binaryImage.rows, binaryImage.step, QImage::Format_RGB888);
    //ImageTool::showImage(colorImage,"ColorImage Image");


    int width = grayImage.cols;
    int height = grayImage.rows;

    QImage targetImage(width, height, QImage::Format_Grayscale8);
    uchar* imagePtr = targetImage.bits();
    int imageBytesPerLine = targetImage.bytesPerLine();

    // 遍历每个像素并复制到QImage中
    for (int y = 0; y < height; ++y) {
        uchar* matRowPtr = grayImage.ptr(y);
        uchar* imageRowPtr = imagePtr + y * imageBytesPerLine;
        memcpy(imageRowPtr, matRowPtr, width);
    }
    ImageTool::showImage(targetImage,"Target Image");
    return targetImage;
}
