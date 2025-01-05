#include "ocrtexttask.h"
#include "globalsignals.h"
#include <QPointF>
OcrTextTask::OcrTextTask(Pix* pix){
    this->pix = pix;
}

void OcrTextTask::run() {
    // 这里执行耗时操作
    QString content;
    bool isOk = readImageText(this->pix,content);
    int code = isOk?MessageCode::OCR_OK:MessageCode::OCR_ERR;
    GlobalSignals::instance()->sendMessageResult(content,code);
    qDebug()<<"OCR识别已处理，消息编码："<<code;
}

/*
bool OcrTextTask::readImageText(Pix *pix,QString &result){
    //tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    tesseract::TessBaseAPI api;
    //Initialize tesseract-ocr with English, without specifying tessdata path
    //const char *datapath = "./tessdata";
    if (api.Init(NULL, "chi_sim+eng",tesseract::OEM_LSTM_ONLY)) {
        result = "无法初始化OCR LSTM引擎，请查看训练数据集文件tessdata是否存在.";
        qDebug()<<result;
        return false;
    }
    api.SetImage(pix);
    char *outText  = api.GetUTF8Text();
    result = QString::fromUtf8(outText);
    api.ClearAdaptiveClassifier();
    api.ClearPersistentCache();
    api.Clear();
    api.End();
    delete [] outText;
    pixDestroy(&pix);
    return true;
}
*/


bool OcrTextTask::readImageText(Pix *pix,QString &result){
    // const char *imgFile = "D:/User/Qt/QtProject/images/lay_word.png";
    //D:/User/Qt/QtProject/images/inclined_letter2.png
    // Pix *image = pixRead(imgFile);
    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    api->Init(NULL, "chi_sim+eng");
    api->SetImage(pix);
    api->Recognize(0);
    tesseract::ResultIterator* ri = api->GetIterator();
    tesseract::PageIteratorLevel level = tesseract::RIL_TEXTLINE;
    QVector<QVector<QPointF>> *regionsPtr =  new QVector<QVector<QPointF>>();;
    if (ri != 0) {
        do {
            const char* word = ri->GetUTF8Text(level);
            // float conf = ri->Confidence(level);
            int left=0, top=0, right=0, bottom=0;
            bool isValidBox = ri->BoundingBox(level, &left, &top, &right, &bottom);
            if(isValidBox){
                QVector<QPointF> points;
                // 左上角
                points.append(QPointF(left, top));
                // 右上角
                points.append(QPointF(right, top));
                // 右下角
                points.append(QPointF(right, bottom));
                // 左下角
                points.append(QPointF(left, bottom));
                regionsPtr->append(points);

                result += QString::fromUtf8(word)+"\n";

                // qDebug()<<"word:"<<QString::fromUtf8(word)<<", conf:"<<conf<<", x1-y2:"<<x1<<","<<y1<<","<<x2<<","<<y2;

            }
            // printf("word: '%s';  \tconf: %.2f; BoundingBox: %d,%d,%d,%d;\n",
            //        word, conf, x1, y1, x2, y2);

            // 用于存储实际裁剪的矩形框
            // BOX* boxClipped;
            // 调用裁剪函数
            // BOX* box = boxCreate(x1,y1,x2-x1,y2-y1);
            // PIX* pixClipped = pixClipRectangle(image, box, &boxClipped);

            // PIX *pixDeskewed = pixDeskew(pixClipped, 1);  // 1 表示自适应校正
            // showPixImage(pixDeskewed);

            // Mat mat = pixToMat(pixDeskewed);
            // cv::imshow("curr region",mat);
            // cv::waitKey(0);
            //


            // l_float32 angle;
            // if (pixGetTextlineAngle(pixDeskewed, 0, 0, 0, &angle, NULL) == 0) {
            //     printf("倾斜角度: %.2f 度\n", angle);
            // }
            // printf("word: '%s';  \tconf: %.2f; BoundingBox: %d,%d,%d,%d;\n",
            //        word, conf, x1, y1, x2, y2);

            delete[] word;
        } while (ri->Next(level));
    }else{
        result = "未识别到有效内容";
        return false;
    }
    delete api;
    GlobalSignals::instance()->sendOcrRegionSelected(regionsPtr);
    // return "finished";
    return true;
}

