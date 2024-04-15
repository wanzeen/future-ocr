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
#include "screencapture.h"
#include "customgraphicsview.h"
#include <QQueue>
#include <QAtomicInt>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public slots:
    // 完成文本识别
    void onRunnableFinished(QString content);
    // 收到截屏图片
    void receiveScreenCapture(QPixmap catureImage);
    void receiveCaptureCancel();
    void receiveGraphicsViewSelected(QPixmap selectedPixmap);
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    // 文本识别内容格式化显示
    QString ocrTextFormat(QString content);
    // 窗口隐藏事件
    // 打开图片事件
    void onOpenButtonClicked();
    // 保存图片
    void onSaveButtonClicked();
    // 复制图片
    void onCopyImageClicked();
    // 识别图片
    void onCaptureButtonClicked();
    //键盘事件监听
    void keyPressEvent(QKeyEvent *event);
    // 提交OCR处理
    void submitOcrTextHandler();
    void wheelEvent(QWheelEvent *event) override;
    // 添加OCR文本处理
    void addOrcTextTask(QPixmap pixmap);
    QString readImageText(QPixmap pixmap);
    //void paintEvent(QPaintEvent *event) override;
    //文本选中事件
    void handleTextSelectionChanged();
    //文本改变事件
    void handleTextChanged();
    //复制已选文本
    void onCopyTextClicked();
    void onAboutClicked();
    void onPasteImageOcrClicked();

private:
    // 创建一个QAtomicInt对象
    int taskCount = 0;
    ScreenCapture *captureScreen = nullptr;
    QPushButton *copyTextBtn;
    QPixmap windowPixmap;
    QPainter painter;
    QLabel *noteLabel;
    qreal angle = 0.0;
    qreal scale = 1.0;
    // 创建一个线程池
    QThreadPool* threadPool = new QThreadPool;

    QQueue<OcrTextTask*> tasks;
    //场景
    QGraphicsScene *scene;
    QPlainTextEdit *edit;
    CustomGraphicsView *view ;
    QTimer *timer;
    QVBoxLayout *centralLayout;
    ScreenCapture *screen;
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
