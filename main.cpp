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
// #include "imagetool.h"
#include "pythonhandler.h"

bool executePaddleOcr(QImage &qImage,QString &result) {
    int nStatus = PyGILState_Check();   //检测当前线程是否拥有GIL
    qDebug()<<"nStatus:"<<nStatus;
    PyGILState_STATE gstate =  PyGILState_Ensure();   //如果没有GIL，则申请获取GIL
    qDebug()<<"gState:"<<gstate;
    const char *pyName = "PaddleOcrTest";
    qDebug()<<"pyName:"<<pyName;
    PyObject* pName = PyUnicode_DecodeFSDefault(pyName);
    PyObject* pModule = PyImport_Import(pName);
        // 释放 pName 对象
    // Py_DECREF(pName);
    // 将OpenCV的Mat数据转换为字节流
    // std::vector<uchar> imageData;
    // cv::imencode(".jpg", cvImage, imageData);
    // 创建Python字节对象
    // PyObject* pImageBytes = PyBytes_FromStringAndSize(reinterpret_cast<const char*>(imageData.data()), imageData.size());
    // 从文件加载QImage
    //QImage qImage("D:/User/Qt/QtProject/images/lay_word.png");
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
        // Save QImage as PNG format
    qImage.save(&buffer, "PNG");
    // PyObject* pythonStr = Py_BuildValue("s#", byteArray.constData(), byteArray.size());
    PyObject* pImageBytes = PyBytes_FromStringAndSize(byteArray.constData(), byteArray.size());
    // 创建参数元组
    PyObject* pArgs = PyTuple_Pack(3, pImageBytes, PyLong_FromLong(qImage.width()), PyLong_FromLong(qImage.height()));
    PyObject* pFunc = nullptr;
    const char *moduleName = "paddleocr_execute";
    if (pModule && PyModule_Check(pModule)) {
        qDebug()<<"已加载函数："<<moduleName;
        pFunc = PyObject_GetAttrString(pModule, moduleName);
        if (pFunc && PyCallable_Check(pFunc)) {
            // 在这里继续执行您的代码
            qDebug()<<"函数可调用";
        } else {
            qDebug()<<"函数无法调用";
            // 处理获取属性失败或不可调用的情况
        }
    } else {
        qDebug()<<"调用函数异常.";
        PyErr_Print();
        // 处理模块对象为空或不是有效的模块对象的情况
    }
    // PyObject* pFunc = PyObject_GetAttrString(pModule, "paddleocr_execute");
    // 调用Python函数
    PyObject* pResult = PyObject_CallObject(pFunc, pArgs);
    if (pResult != NULL) {
        // 在这里处理返回值，例如解析数据或其他操作
        // 这里示例将返回值作为字符串输出到控制台
        if (PyUnicode_Check(pResult)) {
            const char* resultStr = PyUnicode_AsUTF8(pResult);
            qDebug() << "Python返回字符串: " << resultStr;
        }else if(PyList_Check(pResult)) {
            // 解析返回的数据
            int listLength = PyList_Size(pResult);
            qDebug()<<"收到列表数据，长度:"<<listLength;
            for (Py_ssize_t i = 0; i < PyList_Size(pResult); i++) {
                PyObject *pInnerList = PyList_GetItem(pResult, i);
                if (PyList_Check(pInnerList) && PyList_Size(pInnerList)==2 ) {
                    // for (Py_ssize_t j = 0; j < PyList_Size(pInnerList); j++) {
                    PyObject *boxes = PyList_GetItem(pInnerList, 0);

                    qDebug()<<"boxes.size:"<<PyList_Size(boxes);
                    if(PyList_Check(boxes) && PyList_Size(boxes)==4){

                        // PyObject *boxes = PyList_GetItem(items, 0);
                        for (Py_ssize_t k = 0; k < PyList_Size(boxes); k++) {
                            PyObject *point = PyList_GetItem(boxes, k);
                            PyObject *valX = PyList_GetItem(point, 0);
                            double posX =  PyFloat_AsDouble(valX);;
                            qDebug()<<"posX:"<<posX;
                            PyObject *valY = PyList_GetItem(point, 1);
                            double posY =  PyFloat_AsDouble(valY);
                            qDebug()<<"posY:"<<posY;
                        }
                    }
                    PyObject *texts = PyList_GetItem(pInnerList, 1);
                    PyObject *pText = PyTuple_GetItem(texts,0);
                    // 检查pText是否为字符串
                    if (PyUnicode_Check(pText)) {
                        // 获取字符串值
                        const char *textValue = PyUnicode_AsUTF8(pText);
                        result += QString::fromUtf8(textValue)+"\n";
                        qDebug()<<"textValue:"<<textValue;
                    }
                    PyObject *pvalue = PyTuple_GetItem(texts,1);
                    double value =  PyFloat_AsDouble(pvalue);;
                    qDebug()<<"value:"<<value;
                }
                // if (PyTuple_Check(items) && PyTuple_Size(items) == 2) {
                // PyObject *pCoordinates = PyTuple_GetItem(items, 0);
                // int coordinateLength = PyList_Size(pCoordinates);
                // qDebug()<<"coordinateLength:"<<coordinateLength;
                // PyObject *pText = PyTuple_GetItem(pTuple, 1);

                // int pTextLength = PyList_Size(pText);
                // qDebug()<<"pTextLength:"<<pTextLength;
                // 解析坐标
                // 解析文本数据
                // Py_XDECREF(pCoordinates);
                // Py_XDECREF(pText);
                // }
                // }
            }
        }else if(PyBytes_Check(pResult)) {
            qDebug() << "收到字节数据.";
            // 使用PyBytes_AsString()函数获取字节数据的指针
            char* imageData = PyBytes_AsString(pResult);
            if(imageData==nullptr){
                qDebug()<<"图像字节数据为空.";
            }
            Py_ssize_t imageDataSize = PyBytes_Size(pResult);
            qDebug()<<"imageDataSize:"<<imageDataSize;
            // 创建QImage对象并加载图像数据
            // QImage image((uchar*)imageData, qImage.width(), qImage.height(), QImage::Format_RGB888);
            QByteArray byteArray = QByteArray::fromRawData(imageData, imageDataSize);
            QImage image;
            image.loadFromData(byteArray);

            //ImageTool::showImage(image,"Result Image");
            image.save("test.png");
            qDebug()<<"已显示...";

            image.save("test.png");
            qDebug()<<"已显示...";
        }else {
            qDebug() << "python 调用返回数据无法识别." ;
        }

        // 释放返回值对象
        // Py_DECREF(pResult);
    } else {
        qDebug()<<"python 调用出错";
        // 处理调用失败的情况
        PyErr_Print();

    }
    // 释放资源
    Py_DECREF(pResult);
    Py_DECREF(pArgs);
    Py_DECREF(pImageBytes);
    Py_DECREF(pFunc);
    Py_DECREF(pModule);
    return result!=""?true:false;

}

int main(int argc, char *argv[])
{    

    QApplication app(argc, argv);
    // LogSpace::enableLogFileOutput();
    QString currentDirectory = QCoreApplication::applicationDirPath();
    QByteArray tessdataPath = (currentDirectory + "/tessdata").toUtf8();
    // qputenv("TESSDATA_PREFIX",tessdataPath);
    // qDebug()<<"tessdataPath:"<<tessdataPath;
    // 打印当前环境变量，用于验证
    //qDebug() << "TESSDATA_PREFIX:" << QProcessEnvironment::systemEnvironment().value("TESSDATA_PREFIX");
    // PrtScListener pp;
    // python_test(argc,argv);
    // paddle_test();
    PythonHandler handler;
    handler.initPython();

    // QImage qImage("D:/User/Qt/QtProject/images/lay_word.png");
    // QString message;
    // executePaddleOcr(qImage,message);
    // qDebug()<<"message:"<<message;

    MainWindow window;
    QIcon icon(":/icons/FutureOCR.ico");
    LogSpace::setLogFile("D:/User/Qt/QtProject/build-FutureOCR-Desktop_Qt_6_7_0_MSVC2019_64bit-Debug/logs/FutureOCR_log.txt");
    // 设置应用程序窗口的图标
    app.setWindowIcon(icon);
    window.show();
    // 在需要显示错误信息的地方调用以下代码
    // QMessageBox::critical(nullptr, "错误提示", "程序发生一个错误，错误原因请查看日志文件.");
    return app.exec();
}
