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
#include <QToolBar>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    struct PreferenceInfo{
        // 截屏后自动识别
        int captureAutoOcr = 0;
        // 粘贴图像后自动识别
        int pasteImageAutoOcr = 0;
        // 复制图片后最小化窗口
        int copyTextAfterAutoMiniWindow = 0;
        // 启用图像增强
        int enableImageEnhance = 0;
        // int enablePaddleOCR = 1;
    };

public slots:
    // 完成文本识别, code=0 识别成功，code=1 识别出错
    void receiveMessageResult(QString content,int code);
    // 收到截屏图片
    void receiveScreenCapture(QPixmap caturePixmap);
    // 截屏取消事件
    void receiveCaptureCancel();
    // 全局键盘事件
    void receiveGlobalSignalKeyEvent(int eventType);
    // 偏好设置选中事件
    void receivePreferenceCheckBoxChanged(int state);
    // 图像选中
    void receiveGraphicsViewSelected(QPixmap selectedPixmap);
    // 图像添加
    void receiveGraphicsPixmapItemAdded();
    // 图像状态改变
    void receivePixmapChangedDescription(QString description);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    // 工具栏初始化
    void initMenuTool();
    // 图片显示区初始化
    void initImageView();
    // 文本显示区初始化
    void initTextView();
    // 全局键盘事件
    void initGlobalKeyEvent();
    // 设置加载
    void loadPreferenceInfo();
    // 设置选择
    void onPreferenceClicked();

    QString getConfigPath();
    // 文本识别内容格式化显示
    QString ocrTextFormat(QString content);

    // ********** 事件处理函数 **********
    // 打开图片事件
    void onOpenButtonClicked();
    // 保存图片
    void onSaveButtonClicked();
    // 复制图片
    void onCopyImageClicked();
    // 截屏操作
    void onCaptureButtonClicked();
    //键盘事件监听
    void keyPressEvent(QKeyEvent *event);
    // 提交OCR处理
    void onPixmapOcrClicked();
    // 普通函数
    // 添加OCR线程文本处理
    void addOrcTextTask(QPixmap pixmap);
    // 获取灰度化、调整大小后的图片
    QPixmap getAdjustedPixmap(QPixmap &pixmap);
    // 识别图片
    QString readImageText(QPixmap pixmap);
    //void paintEvent(QPaintEvent *event) override;
    //文本选中事件
    void handleTextSelectionChanged();
    //文本改变事件
    void handleTextChanged();
    //复制已选文本
    void onCopyTextClicked();
    // 关于
    void onAboutClicked();
    // 粘贴图片
    void onPasteImageClicked();
    // 保存偏好设置
    bool savePreferenceSetting(); 
protected:
    QSize getSize();
    void setPixmap(QPixmap &pixmap);
private:
    // 设置信息
    PreferenceInfo preferenceInfo;
    // 编辑信息
    PreferenceInfo tempPreferenceInfo;
    QToolBar *toolBar;
    // 菜单栏
    // 截屏OCR(Ctr+P)
    QAction *captureAction;
    // 粘贴图片
    QAction *pasteImageAction;
    // 识别(Ctr+Enter)
    QAction *ocrAction;
    // 保存(Ctr+S)
    QAction *saveAction;
    // 打开(Ctr+O)
    QAction *openAction;
    // 复制图片(Ctr+Shift+C)
    QAction *copyImageAction;
    // 偏好设置
    QAction *preferenceAction;
    // 关于
    QAction *aboutAction;

    // 复制文本
    QPushButton *copyTextBtn;
    // 粘贴并识别图片
    QPushButton *captureBtn;

    // 创建一个QAtomicInt对象
    int taskCount = 0;
    ScreenCapture *captureScreen;
    QPixmap windowPixmap;
    QPainter painter;
    QLabel *noteLabel;
    // 创建一个线程池
    QThreadPool* threadPool;
    //文本显示区
    QPlainTextEdit *edit;
    // 图片显示区
    CustomGraphicsView *imageView ;
    // 全局布局
    QVBoxLayout *centralLayout;
    // 截屏
    ScreenCapture *screen;
    // 线程池最大线程数量
    const int MAX_THREAD_COUNT = 5;
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
