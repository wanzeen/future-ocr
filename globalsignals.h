#ifndef GLOBALSIGNALS_H
#define GLOBALSIGNALS_H
#include <QObject>
#include <QDebug>
#include <QVector>

enum MessageCode{
     OCR_OK = 0,
     OCR_ERR
};

// 识别结果
// struct OcrResult{
//     // 一般为四边形
//     QVector<QPointF> dots;
//     // 识别的准确度
//     dobule accuracy;
//     // 文档内容
//     QString text;
// };

class GlobalSignals:public QObject
{
    Q_OBJECT
signals:
    void globalSignalKeyEvent(int eventType);
    void signalMessageResult(QString content,int code);
    // 程序处理的提示消息
    void globalSignalNoteMessage(QString message);
    void signalOcrRegionSelected(QVector<QVector<QPointF>> *boxes);
public:
    GlobalSignals();
    static GlobalSignals* instance();
    void sendSignalEvent(int eventType);
    void sendOcrRegionSelected(QVector<QVector<QPointF>> *boxes);
    void sendMessageResult(QString &content,int code = MessageCode::OCR_OK);
private:
    static GlobalSignals* globalInstance;
};
#endif // GLOBALSIGNALS_H
