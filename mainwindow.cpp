#include "mainwindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 设置线程池的最大线程数
    threadPool->setMaxThreadCount(4);

    this->setWindowTitle("FutureOCR");


    // 获取屏幕的几何信息
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();

    // 计算窗口居中时的坐标
    int windowWidth = 480;
    int windowHeight = 360;
    int x = screenGeometry.center().x() - windowWidth / 2;
    int y = screenGeometry.center().y() - windowHeight / 2;

    qDebug()<<"x:"<<x<<",y:"<<y<<",cenx:"<<screenGeometry.center().x()<<",ceny:"<<screenGeometry.center().y();
    this->setGeometry(x, y, windowWidth, windowHeight);

    QPushButton *captureBtn  = new QPushButton("截屏OCR(Ctr+P)");
    QPushButton *ocrBtn = new QPushButton("图片识别(Ctr+Enter)");
    QPushButton *saveBtn = new QPushButton("保存(Ctr+S)");
    QPushButton *openBtn = new QPushButton("打开(Ctr+O)");
    QPushButton *copyBtn = new QPushButton("复制(Ctr+Shift+C)");
    // 创建一个垂直布局管理器
    // layout = new QVBoxLayout;

    noteLabel = new QLabel("");
    centralLayout = new QVBoxLayout();

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(captureBtn);
    btnLayout->addWidget(ocrBtn);
    btnLayout->addWidget(saveBtn);
    btnLayout->addWidget(openBtn);
    btnLayout->addWidget(copyBtn);

    centralLayout->addLayout(btnLayout);

    //centralLayout->addWidget(captureBtn);
    //this->setCentralWidget(button);
    // this->setLayout(layout);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(centralLayout);

    this->setCentralWidget(centralWidget);

    // 创建一个 QGraphicsView 对象
     view = new QGraphicsView;
     scene = new QGraphicsScene();
     view -> setScene(scene);
    // 将 QGraphicsView 对象添加到布局管理器
    //layout->addWidget(view);
    //this->layout()->addWidget(view);
    //this->layout->addWidget(view);

    centralLayout->addWidget(view);

    edit = new QPlainTextEdit;
    edit->setPlaceholderText("提示：ctr+v 粘贴图片, ctr+Enter 识别图片，ctr+滚轮缩放图片，ctr+shift+滚轮旋转图片，截屏模式时按Enter识别,按ESC退出");
    centralLayout->addWidget(edit);
    centralLayout->addWidget(noteLabel);
    QObject::connect(captureBtn, &QPushButton::clicked, this, &MainWindow::onCaptureButtonClicked);
    QObject::connect(ocrBtn, &QPushButton::clicked, this, &MainWindow::submitOcrTextHandler);
    QObject::connect(saveBtn, &QPushButton::clicked, this, &MainWindow::onSaveButtonClicked);
    QObject::connect(openBtn, &QPushButton::clicked, this, &MainWindow::onOpenButtonClicked);
    QObject::connect(copyBtn, &QPushButton::clicked, this, &MainWindow::onCopyImageClicked);

    // 显示窗口
    // this->show();
}


MainWindow::~MainWindow()
{
}

