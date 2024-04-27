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
        int captureAutoOcr = 0;
        int pasteImageAutoOcr = 0;
        int copyTextAfterAutoMiniWindow = 0;
    };

public slots:
    // 完成文本识别
    void onRunnableFinished(QString content);
    // 收到截屏图片
    void receiveScreenCapture(QPixmap catureImage);
    void receiveCaptureCancel();
    void receiveGraphicsViewSelected(QPixmap selectedPixmap);
    void receiveGlobalSignalKeyEvent(int eventType);
    void receivePreferenceCheckBoxChanged(int state);
    void receiveGraphicsPixmapItemAdded();

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
    // 截屏操作
    void onCaptureButtonClicked();
    //键盘事件监听
    void keyPressEvent(QKeyEvent *event);
    // 提交OCR处理
    void onPixmapOcrClicked();
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
    // 设置选项
    void onPreferenceClicked();
    // 关于
    void onAboutClicked();
    // 粘贴图片
    void onPasteImageClicked();
    // 保存偏好设置
    bool savePreferenceSetting();
    // 工具栏初始化
    void initMenuTool();
    // 图片显示区初始化
    void initImageView();
    // 文本显示区初始化
    void initTextView();
    // 全局键盘事件
    void initGlobalKeyEvent();
    void loadPreferenceInfo();
    QString getConfigPath();
    QPixmap &adjustPixmap(QPixmap &pixmap);
protected:
    QSize getSize();
    void setPixmap(QPixmap &pixmap);
private:
    PreferenceInfo preferenceInfo;
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
    qreal angle = 0.0;
    qreal scale = 1.0;
    // 创建一个线程池
    QThreadPool* threadPool;
    //场景
    QGraphicsScene *scene;
    //文本显示区
    QPlainTextEdit *edit;
    // 图片显示区
    CustomGraphicsView *view ;
    // 全局布局
    QVBoxLayout *centralLayout;
    // 截屏
    ScreenCapture *screen;
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
