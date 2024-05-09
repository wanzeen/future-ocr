#ifndef GLOBALSIGNALS_H
#define GLOBALSIGNALS_H
#include <QObject>
#include <QDebug>

enum MessageCode{
     OCR_OK = 0,
     OCR_ERR
};

class GlobalSignals:public QObject
{
    Q_OBJECT
signals:
    void globalSignalKeyEvent(int eventType);
    void signalMessageResult(QString content,int code);
    // 程序处理的提示消息
    void globalSignalNoteMessage(QString message);
public:
    GlobalSignals();
    static GlobalSignals* instance();
    void sendSignalEvent(int eventType);
    void sendMessageResult(QString &content,int code = MessageCode::OCR_OK);
private:
    static GlobalSignals* globalInstance;
};
#endif // GLOBALSIGNALS_H
