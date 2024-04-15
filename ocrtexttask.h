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
public:
    OcrTextTask(QPixmap pixmap);
    void run() override;
    QString readImageText(QPixmap &pixmap);
private:
    QPixmap pix;
    int serialNo;
signals:
    void finished(QString content);
};

#endif // OCRTEXTTASK_H
