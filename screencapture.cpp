#include "screencapture.h"

ScreenCapture::ScreenCapture(QWidget *parent):QWidget(parent) {
    setGeometry(100, 100, 400, 400);


    // 截图
    // 获取主屏幕
    QScreen *screen = QGuiApplication::primaryScreen();

    // 截取整个屏幕的像素
    QImage image = screen->grabWindow(0).toImage();

    // // 将图像保存到文件中
    // QImageWriter writer("screenshot.png");

    // writer.write(screenImage);


    // 创建 QImage 对象
    // QImage image("C:/Users/Creator/Pictures/Screenshots/test.png");

    // 创建 QGraphicsScene 对象
    // QGraphicsScene *
    scene = new QGraphicsScene();

    // 将 QImage 添加到 QGraphicsScene
    scene->addPixmap(QPixmap::fromImage(image));

    // // 创建 QGraphicsView 对象
    // QGraphicsView view;
    // // 将 QGraphicsScene 设置为 QGraphicsView
    // view.setScene(&scene);
    // // 显示 QGraphicsView
    // view.show();


    // 创建一个垂直布局管理器
    QVBoxLayout *layout = new QVBoxLayout;

    // 创建一个 QGraphicsView 对象
    QGraphicsView *view = new QGraphicsView;
    view -> setScene(scene);

    // 将 QGraphicsView 对象添加到布局管理器
    layout->addWidget(view);

    this->setLayout(layout);

    // 创建一个窗口并设置其布局管理器
    // QWidget window;
    // this->setLayout(layout);
    // setCentralWidget(view);

    // 显示窗口
    show();
}
