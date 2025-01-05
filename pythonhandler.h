#ifndef PYTHONHANDLER_H
#define PYTHONHANDLER_H
#include <Python.h>
#include <QDebug>
#include <QRunnable>
#include <QImage>
#include <QBuffer>
#include <QSettings>
#include <QCoreApplication>


class PythonHandler
{
public:

    // 初始化之前调用：加载配置
    static void loadIni();

    // 初始化之前调用
    static void setLocation(QString pyHome,QString pySourceDir);
    // 初始化
    static bool init();
    // 关闭
    static void close();


private:
    // python解释器所在目录
    static QString pythonHome;
    // python调用源代码坐在目录
    static QString pyhtonSourceDir;
};


#endif // PYTHONHANDLER_H
