#ifndef OCRTEXTTASK_H
#define OCRTEXTTASK_H

#include <QRunnable>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <QDebug>

class OcrTextTask:public QRunnable
{

public:
    OcrTextTask(Pix* pix);
    void run() override;
    bool readImageText(Pix *pix,QString &result);
private:
    Pix *pix;
};

#endif // OCRTEXTTASK_H
