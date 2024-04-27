#include "winhookwidget.h"
#include "globalsignals.h"
#ifdef Q_OS_WIN
static HHOOK m_hHook;
static bool isCrtPrintScreenHandled = false;
// 全局的键盘钩子回调函数
LRESULT CALLBACK KeyboardHookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        // 处理键盘事件
        KBDLLHOOKSTRUCT* pKeyboard = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        // Ctrl+PrintScreen 被按下
        if (!isCrtPrintScreenHandled && pKeyboard->vkCode == VK_SNAPSHOT && GetAsyncKeyState(VK_CONTROL) & 0x8000)
        {
            isCrtPrintScreenHandled = true;
            // 执行你的操作...
            GlobalSignals::instance()->sendSignalEvent(1);
            //qDebug()<<"keyboard# Ctrl+PrintScreen 被按下";
            /// 返回1表示截取消息不再传递,返回0表示不作处理,消息继续传递
            return 1;
        }
        isCrtPrintScreenHandled = false;
        // if(pKeyboard->vkCode == VK_ESCAPE && GetAsyncKeyState(VK_CONTROL)& 0x8000 && GetAsyncKeyState(VK_SHIFT)&0x8000){
        //     qDebug() << "Ctrl+Shift+Esc";
        // }else if(pKeyboard->vkCode == VK_ESCAPE && GetAsyncKeyState(VK_CONTROL) & 0x8000){
        //     qDebug() << "Ctrl+Esc";
        // }else if(pKeyboard->vkCode == VK_TAB && pKeyboard->flags & LLKHF_ALTDOWN){
        //     qDebug() << "Alt+Tab";
        // }else if(pKeyboard->vkCode == VK_ESCAPE && pKeyboard->flags &LLKHF_ALTDOWN){
        //     qDebug() << "Alt+Esc";
        // }else if(pKeyboard->vkCode == VK_LWIN || pKeyboard->vkCode == VK_RWIN){
        //     qDebug() << "LWIN/RWIN";
        // }else if(pKeyboard->vkCode == VK_F4 && pKeyboard->flags & LLKHF_ALTDOWN){
        //     qDebug() << "Alt+F4";
        // }

    }
    // 调用下一个钩子
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}
WinHookWidget::~WinHookWidget()
{
    // 卸载键盘钩子
    if (m_hHook){
        qDebug()<<"uninstall keyboard hook.";
        UnhookWindowsHookEx(m_hHook);
        m_hHook = nullptr;
    }

}
WinHookWidget::WinHookWidget(QWidget *parent):QWidget(parent) {
    {
        // 创建隐藏窗口
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
        setAttribute(Qt::WA_TranslucentBackground);
        setGeometry(0, 0, 1, 1);
        // 安装键盘钩子
        m_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookCallback, GetModuleHandle(NULL), 0);
        if (m_hHook == NULL)
        {
            qDebug() << "Failed to install keyboard hook.";
        }
        hide();

    }
}
#endif
