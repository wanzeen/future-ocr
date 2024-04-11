#include "mainwindow.h"
#include "screencapture.h"
#include <QProcessEnvironment>



int main(int argc, char *argv[])
{    

   QApplication app(argc, argv);
    QString currentDirectory = QCoreApplication::applicationDirPath();

   QByteArray tessdataPath = (currentDirectory + "/tessdata").toUtf8();
    qputenv("TESSDATA_PREFIX",tessdataPath);
    qDebug()<<"tessdataPath:"<<tessdataPath;
    // 打印当前环境变量，用于验证
    qDebug() << "TESSDATA_PREFIX:" << QProcessEnvironment::systemEnvironment().value("TESSDATA_PREFIX");
   MainWindow window;
   window.show();
   return app.exec();
}
