#ifndef SCREENCAPTURE_H
#define SCREENCAPTURE_H

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsView>

#include <QGuiApplication>
#include <QImageWriter>
#include <QScreen>
#include <QWidget>
#include <QMouseEvent>
#include <QEvent>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTimer>
#include <QMainWindow>

//截屏状态
enum OperateState{
    Init = 0,
    //按下鼠标键盘开始
    Press,
    // 按下并拖动
    Drag,
    //释放鼠标完成
    Finish,
    //点击未拖动
    ClickNoDrag
};

class ScreenCapture:public QWidget
{
    Q_OBJECT
signals:
    //截图完成
    void signalCaptureFinished(QPixmap catureImage);
    void signalCaptureCancel();
public:
    ScreenCapture(QWidget *parent=nullptr);
    void initScreenPixmap();
    void setCapturePreference(bool isAutoOcr);
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    // 快捷键
    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
private:
    QPixmap windowPixmap;
    QPainter painter;
    OperateState currentOperate;
    QPoint startPoint;
    QPoint endPoint;
    QRect lastRect;
    bool isDoubleClick = false;
    bool isAutoOcr = false;
    // QGraphicsScene *scene ;
};

#endif // SCREENCAPTURE_H
