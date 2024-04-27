#ifndef GLOBALSIGNALS_H
#define GLOBALSIGNALS_H
#include <QObject>
#include <QDebug>
class GlobalSignals:public QObject
{
    Q_OBJECT
signals:
    void globalSignalKeyEvent(int eventType);
public:
    GlobalSignals();
    static GlobalSignals* instance();
    void sendSignalEvent(int eventType);
private:
    static GlobalSignals* globalInstance;
};
#endif // GLOBALSIGNALS_H
