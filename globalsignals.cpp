#include "globalsignals.h"
GlobalSignals *GlobalSignals::globalInstance = nullptr;
GlobalSignals::GlobalSignals() {}

GlobalSignals *GlobalSignals::instance()
{
    if(globalInstance==nullptr){
        globalInstance = new GlobalSignals();
    }
    return globalInstance;
}
void GlobalSignals::sendSignalEvent(int eventType){
    emit globalSignalKeyEvent(eventType);
    //qDebug()<<"已发送键盘Ctr键+PrtSc键事件";
}

void GlobalSignals::sendMessageResult(QString &content,int code){
    emit signalMessageResult(content,code);
}


