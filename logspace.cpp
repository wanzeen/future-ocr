#include "logspace.h"
namespace LogSpace {
// 定义日志文件的最大大小限制（单位：字节）
qint64 MAX_LOG_FILE_SIZE = 1024*1024;
// 1MB = 1024 * 1024;
// 日志文件
QString LOG_FILE = "FutureOCR_log.txt";
void checkLogFileSize(const QString& logFilePath)
{
    QFile logFile(logFilePath);
    // 如果日志文件存在且大小超过最大限制
    if (logFile.exists() && logFile.size() > MAX_LOG_FILE_SIZE) {
        // 重命名旧的日志文件
        QString backupFilePath = logFilePath + QDateTime::currentDateTime().toString("_yyyyMMdd_hhmmss") + ".bak";
        logFile.rename(backupFilePath);
        // 删除原日志文件
        logFile.remove();
    }
}

void customMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    QString logMessage;
    // 格式化输出时间
    QString dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    switch (type) {
    case QtDebugMsg:
        logMessage = QString("[%1] [Debug] %2:%3 - %4\n")
                         .arg(dateTime)
                         .arg(context.category)
                         .arg(context.function)
                         .arg(msg);
        break;
    case QtWarningMsg:
        logMessage = QString("[%1] [Warning] %2:%3 - %4\n")
                         .arg(dateTime)
                         .arg(context.category)
                         .arg(context.function)
                         .arg(msg);
        break;
    case QtCriticalMsg:
        logMessage = QString("[%1] [Critical] %2:%3 - %4\n")
                         .arg(dateTime)
                         .arg(context.category)
                         .arg(context.function)
                         .arg(msg);
        break;
    case QtFatalMsg:
        logMessage = QString("[%1] [Fatal] %2:%3 - %4\n")
                         .arg(dateTime)
                         .arg(context.category)
                         .arg(context.function)
                         .arg(msg);
        break;
    }
    QFile outFile(LOG_FILE);
    // 检查日志文件大小
    checkLogFileSize(LOG_FILE);
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream textStream(&outFile);
    textStream << logMessage;
}

void enableLogFileOutput(){
    qInstallMessageHandler(customMessageHandler);
}

// 获取日志文件，默认为：FutureOCR_log.txt
QString getLogFile(){
    return LOG_FILE;
}
// 设置日志文件
void setLogFile(QString logfile){
    // 获取文件所在的目录路径
    QFileInfo fileInfo(logfile);
    QString dirPath = fileInfo.absolutePath();
    qDebug()<<"日志文件路径:"<<dirPath;
    // 创建文件所在的目录路径（如果目录不存在则创建）
    QDir dir;
    if (!dir.mkpath(dirPath)) {
        qDebug() << "无法创建日志路径.";
        return;
    }
    LOG_FILE = logfile;
}
// 设置日志文件大小限制，默认为 2*1024*1024=2M
void setMaxLogLimit(qint64 size){
    MAX_LOG_FILE_SIZE = size;
}
// 获取当前日志文件最大限制
qint64 getMaxLogLimit(){
    return MAX_LOG_FILE_SIZE;
}
}
