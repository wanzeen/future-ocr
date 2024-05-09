#ifndef LOGSPACE_H
#define LOGSPACE_H
#include <QtTypes>
#include <QMessageLogContext>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QDir>

namespace LogSpace {
// 启用日志文件输出
void enableLogFileOutput();
// 获取日志文件，默认为：FutureOCR_log.txt
QString getLogFile();
// 设置日志文件
void setLogFile(QString logfile);
// 设置日志文件大小限制，默认为 2*1024*1024=2M
void setMaxLogLimit(qint64 size);
// 获取当前日志文件最大限制
qint64 getMaxLogLimit();
}

#endif // LOGSPACE_H
