#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFontMetrics>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsView>

#include <QGuiApplication>
#include <QImageWriter>
#include <QScreen>
#include <QWidget>
#include <QMouseEvent>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QTimer>
#include <QPixmap>
#include <QImage>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <capturescreen.h>
#include <string>
#include <QMimeData>
#include <QClipboard>
#include <QApplication>
#include <QGraphicsPixmapItem>
#include <QTransform>
#include <QWheelEvent>
#include <QGraphicsItem>
#include <QLabel>
#include <QThreadPool>
#include "ocrtexttask.h"
#include <QFuture>
#include <QMainWindow>
#include <QFileDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public slots:
    // MainWindow.cpp
    void onRunnableFinished(QString content) {
        // OcrTextTask *runnable = dynamic_cast<OcrTextTask*>(sender());
        QString result = ocrTextFormat(content);
        edit->setPlainText(result);
        edit->appendPlainText("\n\n"+content);
        noteLabel->setText("");
        // 处理返回值
    }
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString ocrTextFormat(QString content){
        if(content.isEmpty()){
            return content;
        }
        int cnt = 0;
        bool isChinese = false;
        QString result;
        int len = content.length() - 1;
        while(cnt<len){
            if(isChinese && content.at(cnt)==' ' && ((content.at(cnt+1).unicode()>=0x4e00 &&  content.at(cnt+1).unicode()<=0x9fa5) || content.at(cnt+1).isPunct())){
                cnt += 1;
                isChinese = false;
                continue;
            }
            result += content[cnt];
            ushort unicode = content.at(cnt).unicode();
            if (unicode >= 0x4e00 && unicode <= 0x9fa5){
                isChinese = true;
            }else{
                isChinese = false;
            }
            cnt += 1;
        }
        return result;
    }
    void hideEvent(QHideEvent *event){
        //qDebug()<<"已隐藏。。。";
    }

    void onOpenButtonClicked(){
        // 打开文件选择器
        QString filePath = QFileDialog::getOpenFileName(this, "选择图片", "", "图片文件 (*.png *.jpg *.jpeg)");
        if (filePath != "") {
            // 创建一个QPixmap对象，并从选择的文件中加载图片
            if(!scene->items().isEmpty()){
                QGraphicsItem *item = scene->items().first();
                scene->removeItem(item);
            }
            QPixmap pixmap(filePath);
            //QGraphicsPixmapItem *pixItem = static_cast<QGraphicsPixmapItem *>(item);
            //pixItem->setPixmap(pixmap);
            scene->addPixmap(pixmap);
            this->setProperty("path",filePath);
            noteLabel->setText("已打开图片："+filePath);

            // 创建一个QLabel对象，并设置其内容为加载的图片
            //QLabel label;
            //label.setPixmap(pixmap);

            // 调整窗口大小以适应图片大小
            //label.resize(pixmap.width(), pixmap.height());

            // 显示窗口
            //label.show();
        }
    }



    void onSaveButtonClicked(){
        if(!scene->items().isEmpty()){
            // 打开文件选择器
            QString filePath = QFileDialog::getSaveFileName(this, "保存为...", "", "图片文件 (*.png *.jpg *.jpeg)");
            if (filePath != "") {
                // 创建一个QPixmap对象，并从选择的文件中加载图片
                QGraphicsPixmapItem *item = static_cast<QGraphicsPixmapItem *>(scene->items().first());
                // 将图片保存到指定的文件路径
                item->pixmap().save(filePath);
                noteLabel->setText("图片保存到："+filePath);

            }
        }else{
            noteLabel->setText("没有可保存的图片.");
        }

    }

    void onCopyImageClicked(){
        if(!scene->items().isEmpty()){
            QGraphicsPixmapItem *item = static_cast<QGraphicsPixmapItem *>(scene->items().first());
            QImage image =  item->pixmap().toImage();
            // 加载图片到QImage对象
            //QImage image("path/to/image.png");

            // 获取系统剪贴板
            QClipboard *clipboard = QApplication::clipboard();
            // 将图片复制到剪贴板
            clipboard->setImage(image);
            noteLabel->setText("图片已复制到粘贴板.");
        }else{
            noteLabel->setText("没有可复制的图片.");
        }

    }

    void onCaptureButtonClicked(){
        // qDebug()<<"onCaptureButtonClicked...";

        this->hide();
        timer = new QTimer(this);
        timer->setInterval(150); //
        timer->setSingleShot(true); // 只执行一次
        QObject::connect(timer, &QTimer::timeout,this, &MainWindow::onIntervalCacheData);
        timer->start();

    }
    void onIntervalCacheData(){
        // 在窗口类的构造函数中
        // qDebug()<<"onIntervalCacheData...";
        CaptureScreen *captureScreen = new CaptureScreen;
        captureScreen->show();
        connect(captureScreen,&CaptureScreen::signalCompleteCature,this, &MainWindow::receiveCapture);
        // qDebug()<<"captrue end..";
        delete timer;
    }

    QString readImageText(QPixmap pixmap){
        tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
        //Initialize tesseract-ocr with English, without specifying tessdata path
        QString content = "";
        if (api->Init(NULL, "eng+chi_sim",tesseract::OEM_LSTM_ONLY)) {
            // qDebug()<<"failed ....";
            fprintf(stderr, "Could not initialize tesseract.\n");
            // exit(1);
            return content;
        }
        QImage qImage = pixmap.toImage();
        Pix *pix = pixCreate(qImage.width(), qImage.height(), 32);
        for (int y = 0; y < qImage.height(); ++y) {
            for (int x = 0; x < qImage.width(); ++x) {
                QColor color = qImage.pixel(x, y);
                pixSetPixel(pix, x, y, color.rgb());
            }
        }
        api->SetImage(pix);
        // Get OCR result
        char *outText  = api->GetUTF8Text();
        content = QString::fromUtf8(outText);
        api->End();
        return content;
    }

    //监听ctr + v 事件
    void keyPressEvent(QKeyEvent *event) {

        if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_O) {
            onOpenButtonClicked();
        }else if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_S) {
            onSaveButtonClicked();
        }else if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_P){
            onCaptureButtonClicked();
        }else if (event->modifiers() & Qt::ControlModifier && (event->modifiers() & Qt::ShiftModifier) && event->key() == Qt::Key_C) {
            onCopyImageClicked();
        }else if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_V) {
            qDebug()<<"监听到ctr+v";
            QClipboard *clipboard = QApplication::clipboard();
            const QMimeData *mimeData = clipboard->mimeData();
            if (mimeData->hasImage()) {
                // if (variant.canConvert<QImage>()) {
                //     QImage image = variant.value<QImage>();
                // }
                QImage image = mimeData->imageData().value<QImage>();
                //QGraphicsScene *scene = static_cast<QGraphicsView*>(object)->scene();

                // 创建一个新的 QGraphicsPixmapItem 并将其添加到场景中
                QGraphicsPixmapItem *item = new QGraphicsPixmapItem(QPixmap::fromImage(image));

                if(!scene->items().isEmpty()){
                    scene->removeItem(scene->items().first());
                }
                //scene->removeItem();
                scene->addItem(item);

                // 创建视图
                // 将场景设置到视图中
                view->setScene(scene);
                // 设置缩放比例
                //view->scale(0.5, 0.5);
                // 将项目居中显示
                item->setPos((scene->width() - item->boundingRect().width()) / 2,
                             (scene->height() - item->boundingRect().height()) / 2);
                qDebug()<<"读取到图片。。。";

            }
        }else if(event->modifiers() == Qt::ControlModifier && ( event->key() == Qt::Key_Return ||  event->key() ==Qt::Key::Key_Enter)){
            submitOcrTextHandler();
        }
    }

    void submitOcrTextHandler(){
        if(!scene->items().isEmpty()){
            //QGraphicsPixmapItem *item = static_cast<QGraphicsPixmapItem *>(scene->items().first());
            // QPixmap pixmap = item->pixmap();

            // pixmap.transformed(transform);

            // QString content = readImageText(item->pixmap());
            // edit->setPlainText(content);

            QGraphicsPixmapItem *item = static_cast<QGraphicsPixmapItem *>(scene->items().first());

            QTransform transform;
            QPixmap pixmap = item->pixmap();
            // 将原点移动到中心
            //transform.translate(pixmap.width() / 2, pixmap.height() / 2);
            transform.rotate(angle);
            //transform.translate(-pixmap.width() / 2, -pixmap.height() / 2); // 将原点移回原位
            pixmap = pixmap.transformed(transform);
            addOrcTextTask(pixmap);
        }else{
            noteLabel->setText("未读取到图片.");
        }
    }
    void wheelEvent(QWheelEvent *event) override
    {


        // 检查是否按下了 Ctrl 键
        if(event->modifiers() & Qt::ControlModifier && !(event->modifiers() & Qt::ShiftModifier)){
            // 检查 Ctrl 键是否按下
            if (event->modifiers() & Qt::ControlModifier) {
                // qDebug()<<"ctr+滚轮";
                // 向前滚动，放大
                if (event->angleDelta().y() > 0) {
                    view->scale(1.2, 1.2);
                    scale += 0.2;
                } else if(event->angleDelta().y()<0) {
                    view->scale(1/1.2,1/1.2);
                    scale -= 0.2;
                }
                noteLabel->setText("缩放大小："+QString::number(scale*100));
                // qDebug()<<"scale:"<<scale;
                // 锁定视图以防止并发修改
                // view->blockSignals(true);
                // view->scale(scale, scale);
                // view->blockSignals(false);
            }
        } else if( event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier))
        {
            // qDebug()<<"Alt...";
            if(!scene->items().isEmpty()){
                // qDebug()<<"not empty...";
                QGraphicsPixmapItem *item = static_cast<QGraphicsPixmapItem *>(scene->items().first());
                // 将旋转点设置为图像中心 item->boundingRect().center()
                //item->setTransformOriginPoint(item->boundingRect().center());
                // 开始旋转


                if(event->angleDelta().y() > 0){
                    angle += 1;
                }else if(event->angleDelta().y()<0) {
                    angle -= 1;
                }
                // qDebug()<<"向上旋转";
                scene->removeItem(item);

                QTransform transform;
                QPixmap pixmap = item->pixmap();
                // 将原点移动到中心
                transform.translate(pixmap.width() / 2, pixmap.height() / 2);
                transform.rotate(angle);
                transform.translate(-pixmap.width() / 2, -pixmap.height() / 2); // 将原点移回原位
                item->setTransform(transform);
                //pixmap->setTransform()
                // item->setPixmap(rotatedPixmap);
                noteLabel->setText("旋转角度："+QString::number(fmod(item->rotation(), 360)));
                scene->addItem(item);
            }
            // // 只按了 Ctrl 键
        }
        // 调用基类处理程序进行默认滚动行为
        //QMainWindow::wheelEvent(event);
    }

    void addOrcTextTask(QPixmap pixmap){
        // 创建一个任务并将其添加到线程池
        noteLabel->setText("正在识别...");

        // connect(task, &OcrTextTask::destroyed, this, [&] {
        //     QString result = task->getResult();
        //     this->edit->setPlainText(result);
        //     // ... 使用结果
        // });

        OcrTextTask* task = new OcrTextTask(pixmap);
        task->setAutoDelete(true);
        connect(task, SIGNAL(finished(QString)), this, SLOT(onRunnableFinished(QString)));
        threadPool->start(task);

        // QFuture<QString> future = QtConcurrent::run(&MainWindow::receiveCapture,pixmap);
        // QFutureWatcher<QString> watcher;
        // watcher.setFuture(future);

        // connect(&watcher, &QFutureWatcher::finished, this,  [=](){
        //     onRunnableFinished(watcher.result());
        //     qDebug() << "finished";
        // });

        // 连接 finished() 信号到槽函数

    }

private slots:
    void receiveCapture(QPixmap catureImage){
        // this->centralWidget()->setVisible(true);
        // this->setVisible(true);
        // 截图
        // 获取主屏幕
        // QScreen *screen = QGuiApplication::primaryScreen();




        // 截取整个屏幕的像素
        //QImage image = screen->grabWindow(0).toImage();

        // 假设 scene 是一个 QGraphicsScene 对象，catureImage 是一个 QPixmap 对象
        // 从场景中删除旧图像
        //QGraphicsItem *oldImage = scene->items();
        if(! scene->items().isEmpty()){
            QGraphicsItem *oldImage = scene->items().first();
            scene->removeItem(oldImage);
            delete oldImage;
        }

        scene->addPixmap(catureImage);
        //QImage image = catureImage.toImage();

        // 将 QImage 添加到 QGraphicsScene


        // // 将图像保存到文件中
        // QImageWriter writer("screenshot.png");

        // writer.write(screenImage);


        // 创建 QImage 对象
        // QImage image("C:/Users/Creator/Pictures/Screenshots/test.png");

        // 创建 QGraphicsScene 对象
        // QGraphicsScene *
        // 将 QImage 添加到 QGraphicsScene
        //scene->addPixmap(QPixmap::fromImage(image));
        // // 创建 QGraphicsView 对象
        // QGraphicsView view;
        // // 将 QGraphicsScene 设置为 QGraphicsView
        // view.setScene(&scene);
        // // 显示 QGraphicsView
        // view.show();


        // 创建一个垂直布局管理器
        //QVBoxLayout *layout = new QVBoxLayout;

        // 创建一个 QGraphicsView 对象
        //view -> setScene(scene);
        // 将 QGraphicsView 对象添加到布局管理器
        //layout->addWidget(view);
        //this->layout()->addWidget(view);
        //this->layout->addWidget(view);
        //this->setLayout(layout);
        //this->setVisible(true);

        // 创建一个窗口并设置其布局管理器
        // QWidget window;
        // this->setLayout(layout);
        // setCentralWidget(view);
        this->show();
        addOrcTextTask(catureImage);

        //QString text = readImageText(catureImage);
        //qDebug()<<"receive data:"<<text;
        //edit->setPlainText(text);
    }
    // QString readImageText_error(QPixmap pixmap){
    //     // Create Tesseract object
    //     tesseract::TessBaseAPI *ocr = new tesseract::TessBaseAPI();

    //     // Initialize OCR engine to use English (eng) and The LSTM OCR engine.
    //     ocr->Init(NULL, "eng", tesseract::OEM_LSTM_ONLY);

    //     // Set Page segmentation mode to PSM_AUTO (3)
    //     ocr->SetPageSegMode(tesseract::PSM_AUTO);

    //     string imPath("C:/Users/Creator/Pictures/Screenshots/test.png");
    //     // Open input image using OpenCV



    //     //QImage image = pixmap.toImage();


    //     Mat im = imread(imPath, IMREAD_COLOR);
    //     // 检查图像是否加载成功
    //     if (im.empty())
    //     {
    //         cerr << "Error loading image!" << endl;
    //         ocr->End();
    //         return "";
    //     }

    //     // 将 QImage 转换为 OpenCV Mat
    //     //Mat im = cv::Mat(image.height(), image.width(), CV_8UC4, (uchar*)image.bits(), image.bytesPerLine());

    //     // 转换颜色空间（可选）
    //     //cv::cvtColor(im, im, COLOR_BGRA2BGR);

    //     // Set image data
    //     ocr->SetImage(im.data, im.cols, im.rows, 3, im.step);

    //     // Run Tesseract OCR on image
    //     string outText = string(ocr->GetUTF8Text());

    //     // print recognized text
    //     //cout << outText << endl;

    //     // Destroy used object and release memory
    //     ocr->End();
    //     return QString(outText.c_str());
    // }


private:
    QLabel *noteLabel;
    qreal angle = 0.0;
    qreal scale = 1.0;
    // 创建一个线程池
    QThreadPool* threadPool = new QThreadPool;

    //场景
    QGraphicsScene *scene;
    QPlainTextEdit *edit;
    QGraphicsView *view ;
    QTimer *timer;
    QVBoxLayout *centralLayout;
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
