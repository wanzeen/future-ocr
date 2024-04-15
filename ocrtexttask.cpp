#include "ocrtexttask.h"

OcrTextTask::OcrTextTask(QPixmap pixmap){
    this->pix = pixmap;
}

void OcrTextTask::run() {
    // 这里执行耗时操作
    //qDebug()<<"start task...";
    QString content = readImageText(this->pix);
    //QMetaObject::invokeMethod(edit,"setPlainText",Q_ARG(QString,content));
    emit finished(content);
}
QString OcrTextTask::readImageText(QPixmap &pixmap){
    //tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();

    tesseract::TessBaseAPI api;

    //Initialize tesseract-ocr with English, without specifying tessdata path
    QString content = "";
    //const char *datapath = "./tessdata";
    if (api.Init(NULL, "chi_sim+eng",tesseract::OEM_LSTM_ONLY)) {
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
    api.SetImage(pix);
    // Get OCR result
    char *outText  = api.GetUTF8Text();
    content = QString::fromUtf8(outText);
    api.ClearAdaptiveClassifier();
    api.ClearPersistentCache();
    api.Clear();
    api.End();
    //delete api;
    //delete [] outText;
    pixDestroy(&pix);
    return content;
}

