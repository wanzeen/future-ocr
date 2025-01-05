#include "paddleocrtask.h"
#include "globalsignals.h"
#include <QJsonDocument>
PaddleOcrTask::PaddleOcrTask(QImage qImage){
    this->image = qImage;
}

void PaddleOcrTask::run(){
    QString content = "";
    bool isOk = executePaddleOcr(this->image,content);
    int code = isOk?MessageCode::OCR_OK:MessageCode::OCR_ERR;
    GlobalSignals::instance()->sendMessageResult(content,code);
}

bool PaddleOcrTask::executePaddleOcr(QImage &qImage,QString &result) {
    //获取全局锁
    PyThreadStateLock pyThreadLock;
    const char *pyName = "paddleocrtask";
    PyObject* pName = PyUnicode_DecodeFSDefault(pyName);
    PyObject *pModule = PyImport_Import(pName);

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
    const char *funcName = "paddleocr_execute";
    qDebug()<<"已加载函数："<<funcName;
    //  && PyModule_Check(pModule)
    bool isOk = false;
    PyObject* pFunc = NULL;
    PyObject* pResult = NULL;
    if (pModule!=NULL) {
        pFunc = PyObject_GetAttrString(pModule, funcName);
        if (pFunc && PyCallable_Check(pFunc)) {
            // 在这里继续执行您的代码
            qDebug()<<"函数可调用";
            pResult = PyObject_CallObject(pFunc, pArgs);
            // 捕获异常并进行处理
            if (PyErr_Occurred()) {
                PyErr_Print(); // 将异常信息输出到标准错误流
                PyErr_Clear(); // 清除异常状态
            }
            // qDebug()<<"callObject...";
            // PyErr_Print();
            // 这里示例将返回值作为字符串输出到控制台
            if (pResult == Py_None) {
                // 如果返回值不是 None，可以继续处理
                qDebug()<<"调用返回值为None.";
                result = "未识别到有效数据.";
                isOk = false;
            } else if(PyList_Check(pResult)) {
                setData(pResult,result);
                isOk = true;
            } else if (PyUnicode_Check(pResult)) {
                PyObject* pUTF8String = PyUnicode_AsUTF8String(pResult);
                if (pUTF8String) {
                    char* resultStr = PyBytes_AsString(pUTF8String);
                    isOk = parseResult(resultStr,result);
                    // qDebug() << "Returned string: " << resultStr;
                    Py_DECREF(pUTF8String);
                }
            }else {
                qDebug()<<"无法识别的数据.";
                if (PyErr_Occurred()){
                    PyErr_Print();
                }
                qDebug() << "python 调用返回数据无法识别." ;
                isOk = false;
            }
        } else {
            // 函数无法调用
            qDebug()<<"调用方法异常.";
            result = "调用方法异常.";
            isOk = false;
            // 处理获取属性失败或不可调用的情况
        }
        //qDebug()<<"del pModule.";
        //Py_XDECREF(pModule);
    } else {
        qDebug()<<"调用模块异常.";
        PyErr_Print();
        result = "调用模块异常.";
        isOk = false;
        // 处理模块对象为空或不是有效的模块对象的情况
    }
    // PyObject* pFunc = PyObject_GetAttrString(pModule, "paddleocr_execute");
    // 调用Python函数
    // qDebug()<<"del pName.";
    // Py_DECREF(pName);
    qDebug()<<"del pName.";
    Py_XDECREF(pName);

    qDebug()<<"del pImageBytes.";
    Py_XDECREF(pImageBytes);

    qDebug()<<"del pModule.";
    Py_XDECREF(pModule);

    qDebug()<<"del pFunc.";
    Py_XDECREF(pFunc);

    qDebug()<<"del pArgs.";
    Py_XDECREF(pArgs);

    qDebug()<<"del pResult.";
    Py_XDECREF(pResult);

    // return result!=""?true:false;
    return isOk;

}


void PaddleOcrTask::setData(PyObject* pResult,QString &result){
    if (PyList_Check(pResult)) {
        // 解析返回的数据
        int listLength = PyList_Size(pResult);
        qDebug()<<"收到列表数据，长度:"<<listLength;
        QVector<QVector<QPointF>> *regionsPtr =  new QVector<QVector<QPointF>>(listLength);;
        for (Py_ssize_t i = 0; i < PyList_Size(pResult); i++) {
            PyObject *pInnerList = PyList_GetItem(pResult, i);
            if (PyList_Check(pInnerList) && PyList_Size(pInnerList)==2 ) {
                // for (Py_ssize_t j = 0; j < PyList_Size(pInnerList); j++) {
                PyObject *boxes = PyList_GetItem(pInnerList, 0);
                qDebug()<<"boxes.size:"<<PyList_Size(boxes);
                int boxSize = PyList_Size(boxes);
                //QVector<QPointF> *region = new QVector<QPointF>();
                QVector<QPointF> region(4);
                if(PyList_Check(boxes) && boxSize== 4){
                    // QVector<QPointF> region(4);
                    // PyObject *boxes = PyList_GetItem(items, 0);
                    for (Py_ssize_t k = 0; k < PyList_Size(boxes); k++) {
                        PyObject *point = PyList_GetItem(boxes, k);

                        PyObject *valX = PyList_GetItem(point, 0);
                        double posX =  PyFloat_AsDouble(valX);
                        // qDebug()<<"del valX.";
                        Py_DECREF(valX);

                        // qDebug()<<"posX:"<<posX;
                        PyObject *valY = PyList_GetItem(point, 1);
                        double posY =  PyFloat_AsDouble(valY);

                        // qDebug()<<"del valY.";
                        Py_DECREF(valY);

                        // qDebug()<<"posY:"<<posY;
                        region[k] = QPointF(posX,posY);

                        // qDebug()<<"decref point.";
                        Py_DECREF(point);

                        // qDebug()<<"del valX.";
                        // Py_XDECREF(valX);

                        // qDebug()<<"del valY.";
                        // Py_XDECREF(valY);
                    }
                }
                // regionsPtr[i] = region;
                (*regionsPtr)[i] = region;
                // qDebug()<<"regionsPtr:"<<(*regionsPtr)[i];
                PyObject *texts = PyList_GetItem(pInnerList, 1);

                PyObject *pText = PyTuple_GetItem(texts,0);
                // 检查pText是否为字符串
                if (PyUnicode_Check(pText)) {
                    // 获取字符串值
                    const char *textValue = PyUnicode_AsUTF8(pText);
                    result += QString::fromUtf8(textValue)+"\n";
                    // qDebug()<<"textValue:"<<textValue;
                    // 这样使用会导致程序崩溃
                    // delete textValue;
                    // free((void*)textValue); // 或者使用 C++ 中的 delete[] textValue（如果使用 new[] 分配内存的话）
                }
                // 此处执行后，第二次会崩溃
                // qDebug()<<"del pText.";
                // Py_DECREF(pText);
                // PyTuple_GetItem函数是获取元组中的一个元素，并不会增加该元素的引用计数
                // PyObject *pvalue = PyTuple_GetItem(texts,1);
                // double value =  PyFloat_AsDouble(pvalue);
                // qDebug()<<"value:"<<value;

                // qDebug()<<"del texts.";
                Py_DECREF(texts);

                // qDebug()<<"del boxes.";
                Py_DECREF(boxes);

                // qDebug()<<"del pvalue.";
                // Py_DECREF(pvalue);

                // qDebug()<<"del pText.";
                // Py_DECREF(pText);
            }
            // qDebug()<<"del pInnerList.";
            Py_DECREF(pInnerList);
        }
        GlobalSignals::instance()->sendOcrRegionSelected(regionsPtr);
    }
}

bool PaddleOcrTask::parseResult(char *data,QString &result){
    bool isOk = false;
    QString jsonString = QString::fromUtf8(data);
    // 将JSON字符串解析为QJsonDocument对象
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
    // 检查JSON文档是否有效
    if (!doc.isNull()) {
        // 获取JSON数组
        QJsonArray jsonArray = doc.array();
        QVector<QVector<QPointF>> *regionsPtr =  new QVector<QVector<QPointF>>(jsonArray.size());;
        int rows = 0;
        // 遍历JSON数组
        for (const QJsonValue &value : jsonArray) {
            // 检查数组元素是否为数组
            if (value.isArray()) {
                // 获取内部数组
                QJsonArray nestedArray = value.toArray();
                QVector<QPointF> points(4);
                isOk = true;
                // 遍历内部数组
                // for (const QJsonValue &innerValue : innerArray) {
                // 检查内部数组元素是否为数组
                // if (innerArray.isArray()) {
                // 获取二级内部数组
                // QJsonArray nestedArray = innerArray.toArray();
                // QJsonArray nestedArray = innerValue.at(0).toArray();
                // 输出二级内部数组元素
                if(nestedArray.size()==2){
                    QJsonValue boxesJson = nestedArray.at(0);
                    if(boxesJson.isArray()){
                        QJsonArray boxes = boxesJson.toArray();
                        if(boxes.size()==4){
                            int cnt = 0;
                            for (const QJsonValue &pointValue : boxes) {
                                QJsonArray point = pointValue.toArray();
                                points[cnt] = QPointF(point.at(0).toDouble(0),point.at(1).toDouble(0));
                                // qDebug()<<"point:"<<points[cnt];
                                cnt += 1;
                            }
                        }
                    }
                    QJsonValue textJson = nestedArray.at(1);
                    QJsonArray textValue = textJson.toArray();
                    result += textValue.at(0).toString()+"\n";
                    // qDebug()<<"textValue:"<<textValue.at(0).toString();
                    (*regionsPtr)[rows] = points;
                    rows +=1;
                    // }
                }

            }
        }
        GlobalSignals::instance()->sendOcrRegionSelected(regionsPtr);
    } else {
        // 输出错误信息
        // qDebug() << "Invalid JSON document";
        result  = "未识别到有效数据.";
        isOk = false;
    }
    return isOk;

}

