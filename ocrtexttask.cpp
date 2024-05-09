#include "ocrtexttask.h"
#include "globalsignals.h"

OcrTextTask::OcrTextTask(Pix* pix){
    this->pix = pix;
}

void OcrTextTask::run() {
    // 这里执行耗时操作
    QString content;
    bool isOk = readImageText(this->pix,content);
    int code = isOk?MessageCode::OCR_OK:MessageCode::OCR_ERR;
    GlobalSignals::instance()->sendMessageResult(content,code);
    qDebug()<<"OCR识别已处理，消息编码："<<code<<", 消息内容："<<content;
}


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

