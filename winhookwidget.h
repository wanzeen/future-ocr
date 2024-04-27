#ifndef WINHOOKWIDGET_H
#define WINHOOKWIDGET_H
#include <QCoreApplication>
#include <QWidget>
#include <QKeyEvent>
#ifdef Q_OS_WIN
#include <Windows.h>
class WinHookWidget : public QWidget
{
    Q_OBJECT
signals:
    // eventType 取值：
    // 1: Crt键+PrtSc键 截屏事件
    // 2:
    void signalGlobalKeyEvent(int eventType);

public:
    WinHookWidget(QWidget *parent=nullptr);
    ~WinHookWidget();
};
// HHOOK WinHookWidget::m_hHook=NULL;
#endif
#endif // WINHOOKWIDGET_H
