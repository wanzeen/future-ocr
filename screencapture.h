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

class ScreenCapture:public QWidget
{
    Q_OBJECT
public:
    ScreenCapture(QWidget *parent=nullptr);

protected:
    // void mousePressEvent(QMouseEvent *event);
    // void mouseMoveEvent(QMouseEvent* event);
    // void mouseReleaseEvent(QMouseEvent *event);
    // void keyPressEvent(QKeyEvent *event);

    void mousePressEvent(QMouseEvent *event) /*override*/ {
        qDebug()<<"press";
        eventHandler(event);
        return QWidget::mousePressEvent(event);
    }
    void mouseMoveEvent(QMouseEvent* event) /*override*/ {
        qDebug()<<"move...";
        eventHandler(event);
        return QWidget::mouseMoveEvent(event);
    }
    void mouseReleaseEvent(QMouseEvent *event) /*override*/{
        qDebug()<<"release..";
        eventHandler(event);
        return QWidget::mouseReleaseEvent(event);
    }

    void eventHandler( QMouseEvent *event)
    // bool event(QEvent *event) override
    {
        qDebug()<<"eventType: "<<event->type();

        // 处理鼠标事件
        if (event->type() == QMouseEvent::MouseButtonPress)
        {
            qDebug()<<"mouse press... ";

            // 开始记录鼠标移动事件
            m_isRecording = true;

            // QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

            // 获取鼠标按下的位置
            m_startPos = event->pos();
        }
        else if (event->type() == QMouseEvent::MouseMove)
        {
            qDebug()<<"mouse move... ";

            // 更新选定的区域
            if (m_isRecording)
            {
                // QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

                m_endPos = event->pos();
                update();
            }
        }
        else if (event->type() == QMouseEvent::MouseButtonRelease)
        {
            qDebug()<<"mouse release... ";

            // 停止记录鼠标移动事件
            m_isRecording = false;

            // 获取选定区域的屏幕截图
            QScreen *screen = QGuiApplication::primaryScreen();
            // QRect rect(m_startPos, m_endPos);

            //    QPixmap grabWindow(WId window = 0, int x = 0, int y = 0, int w = -1, int h = -1);
            int width = m_endPos.x() - m_startPos.x();
            int height = m_endPos.y() - m_startPos.y();
            QPixmap pixmap = screen->grabWindow(winId(),m_startPos.x(),m_startPos.y(),width,height );

            // 将像素图转换为图像
            QImage image = pixmap.toImage();

            // 将图像保存到文件中
            image.save("screenshot.png");

            qDebug()<<"save...";
            scene->addPixmap(QPixmap::fromImage(image));
            update();

        }

        // return false;
        //QWindow::event(event);
    }

    void paintEvent(QPaintEvent *event) override
    {
        qDebug()<<"draw:"<<m_isRecording;
        // 绘制选定的区域
        if (m_isRecording)
        {
            QPainter painter(this);
            painter.setPen(QPen(Qt::red, 2));
            painter.drawRect(QRect(m_startPos, m_endPos));
        }
    }

private:
    bool m_isRecording = false;
    QPoint m_startPos;
    QPoint m_endPos;
    QGraphicsScene *scene ;
};

#endif // SCREENCAPTURE_H
