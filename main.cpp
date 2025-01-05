#include "mainwindow.h"
#include "screencapture.h"
#include "capturescreen.h"
#include <QProcessEnvironment>
#include <QMessageBox>
#include<QDebug>
#include <iostream>
#include<string>
#include <QBuffer>
using namespace std;
#include "logspace.h"
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "pythonhandler.h"
/*



#include "paddleocrtask.h"
void paddleocrTest(){
    PythonHandler::init();
    QImage image("D:/User/images/inclined_letter.png");
    PaddleOcrTask paddleTask(image);
    QString result;
    paddleTask.executePaddleOcr(image,result);
    qDebug()<<"result:"<<result;
}
*/



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    LogSpace::enableLogFileOutput();
    QString currentDirectory = QCoreApplication::applicationDirPath();
    currentDirectory = "D:/User/Qt/QtProject/FutureOCR";
    qDebug()<<"currentDirectory:"<<currentDirectory;
    QByteArray tessdataPath = (currentDirectory + "/tessdata").toUtf8();
    qputenv("TESSDATA_PREFIX",tessdataPath);
    qDebug()<<"tessdataPath:"<<tessdataPath;
    // 打印当前环境变量，用于验证
    //qDebug() << "TESSDATA_PREFIX:" << QProcessEnvironment::systemEnvironment().value("TESSDATA_PREFIX");
    PythonHandler::loadIni();
    // PythonHandler::init();
    MainWindow window;
    QIcon icon(":/icons/FutureOCR.ico");
    // LogSpace::setLogFile("D:/User/Qt/QtProject/build-FutureOCR-Desktop_Qt_6_7_0_MSVC2019_64bit-Debug/logs/FutureOCR_log.txt");
    // 设置应用程序窗口的图标
    app.setWindowIcon(icon);
    window.show();

    // paddleocrTest();
    // 在需要显示错误信息的地方调用以下代码
    // QMessageBox::critical(nullptr, "错误提示", "程序发生一个错误，错误原因请查看日志文件.");
    int code = app.exec();
    qDebug()<<"程序终止, code:"<<code;
    PythonHandler::close();
    return code;
}

