#ifndef PADDLEOCRTASK_H
#define PADDLEOCRTASK_H
#include <QRunnable>
#include <QDebug>
#include <QImage>
#include <QBuffer>
// #include <Python.h>
#include "pythreadstatelock.h"
// #include "pythonhandler.h"
#include <QJsonArray>

class PaddleOcrTask:public QRunnable{
private:
    QImage image;
public:
    PaddleOcrTask(QImage qImage);

    void run() override;

    bool executePaddleOcr(QImage &qImage,QString &result);

    void setData(PyObject* pResult,QString &result);
    bool parseResult(char *data,QString &result);
};
#endif // PADDLEOCRTASK_H
