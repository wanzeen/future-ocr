#ifndef OCRTEXTTASK_H
#define OCRTEXTTASK_H

#include <QRunnable>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <QPixmap>
#include <QDebug>
#include <QObject>
#include <QPlainTextEdit>

class OcrTextTask: public QObject, public QRunnable
{

    Q_OBJECT

signals:
    void finished(QString content);

public:


    OcrTextTask(QPixmap pixmap);

    void run() override {
        // 这里执行耗时操作
        qDebug()<<"start task...";
        content = readImageText(this->pix);
        //QMetaObject::invokeMethod(edit,"setPlainText",Q_ARG(QString,content));

        emit finished(content);

    }
    //void finished(QString content){}

    // QString getResult(){
    //     return content;
    // }
    QString readImageText(QPixmap pixmap){
        tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
        //Initialize tesseract-ocr with English, without specifying tessdata path
        QString content = "";
        //const char *datapath = "./tessdata";
        if (api->Init(NULL, "eng+chi_sim",tesseract::OEM_LSTM_ONLY)) {
            qDebug()<<"failed ....";
            fprintf(stderr, "Could not initialize tesseract.\n");
            // exit(1);
            return content;
        }
        QImage qImage = pixmap.toImage();
        Pix *pix = pixCreate(qImage.width(), qImage.height(), 32);
        for (int y = 0; y < qImage.height(); ++y) {
            for (int x = 0; x < qImage.width(); ++x) {
                QColor color = qImage.pixel(x, y);
                pixSetPixel(pix, x, y, color.rgb());
            }
        }
        api->SetImage(pix);
        // Get OCR result
        char *outText  = api->GetUTF8Text();
        content = QString::fromUtf8(outText);
        api->End();
        return content;
    }
private:
    QString content;
    QPixmap pix;

};

#endif // OCRTEXTTASK_H
