#include "mainwindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include "screencapture.h"
#include <QFuture>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 设置线程池的最大线程数
    threadPool->setMaxThreadCount(4);

    this->setWindowTitle("FutureOCR");

    QPushButton *captureBtn  = new QPushButton("截屏OCR(Ctr+P)");
    QPushButton *ocrBtn = new QPushButton("识别(Ctr+Enter)");
    QPushButton *saveBtn = new QPushButton("保存(Ctr+S)");
    QPushButton *openBtn = new QPushButton("打开(Ctr+O)");
    QPushButton *copyBtn = new QPushButton("复制图片(Ctr+Shift+C)");
    QPushButton *aboutBtn = new QPushButton("关于");

    copyTextBtn = new QPushButton("复制已选文本");

    QPushButton *pasteImageOcrBtn = new QPushButton("粘贴图像OCR");


    // 创建一个垂直布局管理器
    // layout = new QVBoxLayout;

    noteLabel = new QLabel("");

    // 创建调色板对象
    QPalette palette;
    // 设置前景色为浅褐色
    palette.setColor(QPalette::WindowText, QColor(51, 51, 51));
    // 应用调色板到QLabel
    noteLabel->setPalette(palette);


    centralLayout = new QVBoxLayout();

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(captureBtn);
    btnLayout->addWidget(ocrBtn);
    btnLayout->addWidget(saveBtn);
    btnLayout->addWidget(openBtn);
    btnLayout->addWidget(copyBtn);
    btnLayout->addWidget(aboutBtn);


    //添加顶部工具按钮
    centralLayout->addLayout(btnLayout);
    //this->setCentralWidget(button);
    // this->setLayout(layout);

    // QSize textSize = fontMetrics.size(Qt::TextSingleLine, captureBtn->text());
    // int buttonWidth = textSize.width() + 16;  // 加上一些额外的空间
    // int buttonHeight = textSize.height() + 8;


    //QFont font("Arial", 12); // 使用指定字体和字号
    QFontMetrics fontMetrics(this->font());
    //int charWidth = fontMetrics.horizontalAdvance('H');
    //qDebug()<<"charWidht:"<<charWidth;
    QSize size = fontMetrics.size(Qt::TextSingleLine,"汉");
    //qDebug()<<"size--w:"<<size.width()<<", h:"<<size.height();

    int areaWidth = size.width() * 20;
    int areaHeight = size.height() * 10;
    //qDebug()<<"areaSize:"<<areaWidth<<", h:"<<areaHeight;


    // 获取屏幕的几何信息
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();

    // 计算窗口居中时的坐标
    int windowWidth = areaWidth;
    int windowHeight = areaHeight;
    int x = screenGeometry.center().x() - windowWidth / 2;
    int y = screenGeometry.center().y() - windowHeight / 2;
    //qDebug()<<"x:"<<x<<",y:"<<y<<",cenx:"<<screenGeometry.center().x()<<",ceny:"<<screenGeometry.center().y();
    // this->setGeometry(x, y, windowWidth, windowHeight);


    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(centralLayout);


    this->setCentralWidget(centralWidget);

    // 创建一个 QGraphicsView 对象
    view = new CustomGraphicsView(this);
     view->setMinimumSize(QSize(areaWidth,areaHeight));

     scene = new QGraphicsScene();
     view -> setScene(scene);
    // 将 QGraphicsView 对象添加到布局管理器
    //layout->addWidget(view);
    //this->layout()->addWidget(view);
    //this->layout->addWidget(view);

    QHBoxLayout *captureLayout = new QHBoxLayout();
    captureLayout->addWidget(view);
    //添加图片显示区
    centralLayout->addLayout(captureLayout);

    edit = new QPlainTextEdit();
    edit->setMinimumSize(areaWidth,areaHeight);
    edit->setPlaceholderText("识别内容");
    //添加文本显示区
    centralLayout->addWidget(edit);

     QHBoxLayout *textOperateLayout = new QHBoxLayout();
    textOperateLayout->addWidget(copyTextBtn);
    textOperateLayout->addWidget(pasteImageOcrBtn);

    centralLayout->addLayout(textOperateLayout);
    copyTextBtn->setEnabled(false);

    noteLabel->setAlignment(Qt::AlignCenter);
    centralLayout->addWidget(noteLabel);
    QObject::connect(captureBtn, &QPushButton::clicked, this, &MainWindow::onCaptureButtonClicked);
    QObject::connect(ocrBtn, &QPushButton::clicked, this, &MainWindow::submitOcrTextHandler);
    QObject::connect(saveBtn, &QPushButton::clicked, this, &MainWindow::onSaveButtonClicked);
    QObject::connect(openBtn, &QPushButton::clicked, this, &MainWindow::onOpenButtonClicked);
    QObject::connect(copyBtn, &QPushButton::clicked, this, &MainWindow::onCopyImageClicked);
    QObject::connect(aboutBtn, &QPushButton::clicked, this, &MainWindow::onAboutClicked);

    //文本选中状态改变
    connect(edit, &QPlainTextEdit::selectionChanged, this, &MainWindow::handleTextSelectionChanged);
    connect(edit, &QPlainTextEdit::textChanged, this, &MainWindow::handleTextChanged);
    //复制文本按钮
    QObject::connect(copyTextBtn, &QPushButton::clicked, this, &MainWindow::onCopyTextClicked);
    //粘贴文本按钮
    QObject::connect(pasteImageOcrBtn, &QPushButton::clicked, this, &MainWindow::onPasteImageOcrClicked);
    //图片选中
    connect(view,&CustomGraphicsView::signalGraphicsViewSelected,this,&MainWindow::receiveGraphicsViewSelected);

    // 显示窗口
    // this->show();
}

void MainWindow::handleTextChanged(){
    if(edit->toPlainText()!=""){
        copyTextBtn->setEnabled(true);
        if(!edit->textCursor().hasSelection()){
            copyTextBtn->setText("复制文本");
        }else{
            copyTextBtn->setText("复制已选文本");
        }
    }else{
        copyTextBtn->setText("复制文本");
        copyTextBtn->setEnabled(false);
    }
}
void MainWindow::receiveGraphicsViewSelected(QPixmap selectedPixmap){
    addOrcTextTask(selectedPixmap);
}
void MainWindow::onPasteImageOcrClicked(){
    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    if (mimeData->hasImage()) {
        // if (variant.canConvert<QImage>()) {
        //     QImage image = variant.value<QImage>();
        // }
        QPixmap pixmap = qvariant_cast<QPixmap>(mimeData->imageData());
        addOrcTextTask(pixmap);
        // 创建一个新的 QGraphicsPixmapItem 并将其添加到场景中
        QGraphicsPixmapItem *item = new QGraphicsPixmapItem(pixmap);

        if(!scene->items().isEmpty()){
            scene->removeItem(scene->items().first());
            view->resetMouseState();
        }
        //scene->removeItem();
        scene->addItem(item);
        // 创建视图
        // 将场景设置到视图中
        view->setScene(scene);
        // 设置缩放比例
        //view->scale(0.5, 0.5);
        // 将项目居中显示
        item->setPos((scene->width() - item->boundingRect().width()) / 2,
                     (scene->height() - item->boundingRect().height()) / 2);
    }else{
       noteLabel->setText("未获取到粘贴板图片.");
    }

}
void MainWindow::onCopyTextClicked(){
     QClipboard *clipboard = QApplication::clipboard();
    if(edit->textCursor().selectedText()!=""){
       QString textToCopy = edit->textCursor().selectedText();
       clipboard->setText(textToCopy);
       noteLabel->setText("选中文本已复制.");
    }else if(edit->toPlainText()!=""){
       clipboard->setText(edit->toPlainText());
       noteLabel->setText("所有文本已复制.");
    }
}
void MainWindow::handleTextSelectionChanged(){
    if(edit->textCursor().hasSelection()){
        copyTextBtn->setText("复制已选文本");
    }else{
        copyTextBtn->setText("复制文本");
    }
}
void MainWindow::onRunnableFinished(QString content) {
    QString result = ocrTextFormat(content);
    if(result!=edit->toPlainText()){
        qDebug()<<"result set...";
        edit->setPlainText(result);
    }
    taskCount -= 1;
    qDebug()<<"finished taskCount:"<<taskCount;
    if(taskCount==0){
        noteLabel->setText("识别完成.");
    }
}

QString MainWindow::ocrTextFormat(QString content){
    if(content.isEmpty()){
        return content;
    }
    int cnt = 0;
    bool isChinese = false;
    QString result;
    int len = content.length() - 1;
    while(cnt<len){
        if(isChinese && content.at(cnt)==' ' && ((content.at(cnt+1).unicode()>=0x4e00 &&  content.at(cnt+1).unicode()<=0x9fa5) || content.at(cnt+1).isPunct())){
            cnt += 1;
            isChinese = false;
            continue;
        }
        result += content[cnt];
        ushort unicode = content.at(cnt).unicode();
        if (unicode >= 0x4e00 && unicode <= 0x9fa5){
            isChinese = true;
        }else{
            isChinese = false;
        }
        cnt += 1;
    }
    return result;
}
// void MainWindow::hideEvent(QHideEvent *event){
//     // QMainWindow::hideEvent(event);
//     qDebug()<<"hideEvent...";
// }
void MainWindow::onAboutClicked(){
    // 创建一个QDialog
    QDialog dialog;
    dialog.setWindowTitle("关于FutureOCR");
    // 创建一个QLabel
    QLabel label;
    // 设置固定宽度
    //label.setFixedWidth(300);
    // 启用自动换行
    label.setWordWrap(true);
    // 设置文本可被鼠标选中
    label.setTextInteractionFlags(Qt::TextSelectableByMouse);
    QString content = "关于FutureOCR\n\n";
    content +="版本：FutureOCR_V1.0.0\n";
    content +="介绍：FutureOCR一个光学文本识别工具，可实现对截图或图片内容中的中英文内容识别，支持图片任意范围内容选中识别，识别功能采用多线程支持，具有较快的响应速度\n"
               "    该软件是在Github最新开源库Tesseract的基础上研发设计，这是一个基于神经网络（LSTM） 的 OCR 引擎\n";
    content += "    对于标准的中英文字体，具有较好的识别度，对于中文斜体字的识别度欠佳\n";
    content += "    FutureOCR由C++语言开发，通过Qt6.7开发工具设计\n";
    content += "操作提示：\n";
    content += "    1.图像显示区【Ctrl】+【Shift】+【鼠标滚轮】组合键，可实现图片旋转，旋转正的图片再识别具有较好的识别效果\n";
    content += "    2.图像显示区【Ctrl】+【鼠标滚轮】组合键，可实现图片放大缩小\n";
    content += "    3.图像显示区【Ctrl】+【V】组合键，可实现图片直接粘贴\n";
    content += "作者：wangzeng\n";
    label.setText(content);

    // 创建一个垂直布局管理器
    QVBoxLayout layout(&dialog);
    layout.addWidget(&label);

    // 显示对话框
    dialog.exec();

}

void MainWindow::onOpenButtonClicked(){
    // 打开文件选择器
    QString filePath = QFileDialog::getOpenFileName(this, "选择图片", "", "图片文件 (*.png *.jpg *.jpeg)");
    if (filePath != "") {
        // 创建一个QPixmap对象，并从选择的文件中加载图片
        if(!scene->items().isEmpty()){
            QGraphicsItem *item = scene->items().first();
            scene->removeItem(item);
        }
        QPixmap pixmap(filePath);
        //QGraphicsPixmapItem *pixItem = static_cast<QGraphicsPixmapItem *>(item);
        //pixItem->setPixmap(pixmap);
        scene->addPixmap(pixmap);
        this->setProperty("path",filePath);
        noteLabel->setText("已打开图片："+filePath);

        // 创建一个QLabel对象，并设置其内容为加载的图片
        //QLabel label;
        //label.setPixmap(pixmap);

        // 调整窗口大小以适应图片大小
        //label.resize(pixmap.width(), pixmap.height());

        // 显示窗口
        //label.show();
    }
}



void MainWindow::onSaveButtonClicked(){
    if(!scene->items().isEmpty()){
        // 打开文件选择器
        QString filePath = QFileDialog::getSaveFileName(this, "保存为...", "", "图片文件 (*.png *.jpg *.jpeg)");
        if (filePath != "") {
            // 创建一个QPixmap对象，并从选择的文件中加载图片
            QGraphicsPixmapItem *item = static_cast<QGraphicsPixmapItem *>(scene->items().first());
            // 将图片保存到指定的文件路径
            item->pixmap().save(filePath);
            noteLabel->setText("图片保存到："+filePath);

        }
    }else{
        noteLabel->setText("没有可保存的图片.");
    }

}

void MainWindow::onCopyImageClicked(){
    if(!scene->items().isEmpty()){
        QGraphicsPixmapItem *item = static_cast<QGraphicsPixmapItem *>(scene->items().first());
        QImage image =  item->pixmap().toImage();
        // 加载图片到QImage对象
        //QImage image("path/to/image.png");
        // 获取系统剪贴板
        QClipboard *clipboard = QApplication::clipboard();
        // 将图片复制到剪贴板
        clipboard->setImage(image);
        noteLabel->setText("图片已复制到粘贴板.");
    }else{
        noteLabel->setText("没有可复制的图片.");
    }
}
// void MainWindow::paintEvent(QPaintEvent *event)
// {
//     QMainWindow::paintEvent(event);
// }

void MainWindow::onCaptureButtonClicked(){
    this->setWindowOpacity(0);
    if(captureScreen==nullptr){
        captureScreen = new ScreenCapture();
        //惊天bug，此处必须判断，不然出现重复绑定导致多次执行异常
        connect(captureScreen,&ScreenCapture::signalCaptureFinished,this, &MainWindow::receiveScreenCapture);
        connect(captureScreen,&ScreenCapture::signalCaptureCancel,this,&MainWindow::receiveCaptureCancel);
    }
    captureScreen->initScreenPixmap();
    captureScreen->show();
}

// QString MainWindow::readImageText(QPixmap pixmap){
//     tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
//     //Initialize tesseract-ocr with English, without specifying tessdata path
//     QString content = "";
//     if (api->Init(NULL, "eng+chi_sim",tesseract::OEM_LSTM_ONLY)) {
//         // qDebug()<<"failed ....";
//         fprintf(stderr, "Could not initialize tesseract.\n");
//         // exit(1);
//         return content;
//     }
//     QImage qImage = pixmap.toImage();
//     Pix *pix = pixCreate(qImage.width(), qImage.height(), 32);
//     for (int y = 0; y < qImage.height(); ++y) {
//         for (int x = 0; x < qImage.width(); ++x) {
//             QColor color = qImage.pixel(x, y);
//             pixSetPixel(pix, x, y, color.rgb());
//         }
//     }
//     api->SetImage(pix);
//     // Get OCR result
//     char *outText  = api->GetUTF8Text();
//     content = QString::fromUtf8(outText);
//     api->End();
//     return content;
// }
QString MainWindow::readImageText(QPixmap pixmap){
    //tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    tesseract::TessBaseAPI api;
    //Initialize tesseract-ocr with English, without specifying tessdata path
    QString content = "";
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


//监听ctr + v 事件
void MainWindow::keyPressEvent(QKeyEvent *event) {

    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_O) {
        onOpenButtonClicked();
    }else if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_S) {
        onSaveButtonClicked();
    }else if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_P){
        onCaptureButtonClicked();
    }else if (event->modifiers() & Qt::ControlModifier && (event->modifiers() & Qt::ShiftModifier) && event->key() == Qt::Key_C) {
        onCopyImageClicked();
    }else if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_V) {
        onPasteImageOcrClicked();
    }else if(event->modifiers() == Qt::ControlModifier && ( event->key() == Qt::Key_Return ||  event->key() ==Qt::Key::Key_Enter)){
        submitOcrTextHandler();
    }
}


void MainWindow::submitOcrTextHandler(){
    if(!scene->items().isEmpty()){
        //QGraphicsPixmapItem *item = static_cast<QGraphicsPixmapItem *>(scene->items().first());
        // QPixmap pixmap = item->pixmap();
        // pixmap.transformed(transform);
        // QString content = readImageText(item->pixmap());
        // edit->setPlainText(content);
        QGraphicsPixmapItem *item = static_cast<QGraphicsPixmapItem *>(scene->items().first());
        QTransform transform;
        QPixmap pixmap = item->pixmap();
        // 将原点移动到中心
        //transform.translate(pixmap.width() / 2, pixmap.height() / 2);
        transform.rotate(angle);
        //transform.translate(-pixmap.width() / 2, -pixmap.height() / 2); // 将原点移回原位
        pixmap = pixmap.transformed(transform);
        addOrcTextTask(pixmap);
    }else{
        noteLabel->setText("未读取到图片.");
    }
}
void MainWindow::wheelEvent(QWheelEvent *event)
{
    // 检查是否按下了 Ctrl 键
    if(event->modifiers() & Qt::ControlModifier && !(event->modifiers() & Qt::ShiftModifier)){
        // 检查 Ctrl 键是否按下
        if (event->modifiers() & Qt::ControlModifier) {
            // qDebug()<<"ctr+滚轮";
            // 向前滚动，放大
            if (event->angleDelta().y() > 0) {
                view->scale(1.2, 1.2);
                scale += 0.2;
            } else if(event->angleDelta().y()<0) {
                view->scale(1/1.2,1/1.2);
                scale -= 0.2;
            }
            noteLabel->setText("缩放大小："+QString::number(scale*100));
            // qDebug()<<"scale:"<<scale;
            // 锁定视图以防止并发修改
            // view->blockSignals(true);
            // view->scale(scale, scale);
            // view->blockSignals(false);
        }
    } else if( event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier))
    {
        if(!scene->items().isEmpty()){
            // qDebug()<<"not empty...";
            QGraphicsPixmapItem *item = static_cast<QGraphicsPixmapItem *>(scene->items().first());
            // 将旋转点设置为图像中心 item->boundingRect().center()
            //item->setTransformOriginPoint(item->boundingRect().center());
            // 开始旋转


            if(event->angleDelta().y() > 0){
                angle += 1;
            }else if(event->angleDelta().y()<0) {
                angle -= 1;
            }
            // qDebug()<<"向上旋转";
            scene->removeItem(item);

            QTransform transform;
            QPixmap pixmap = item->pixmap();
            // 将原点移动到中心
            transform.translate(pixmap.width() / 2, pixmap.height() / 2);
            transform.rotate(angle);
            transform.translate(-pixmap.width() / 2, -pixmap.height() / 2); // 将原点移回原位
            item->setTransform(transform);
            //pixmap->setTransform()
            // item->setPixmap(rotatedPixmap);
            noteLabel->setText("旋转角度："+QString::number(fmod(angle, 360)));
            scene->addItem(item);
        }
    }
}

void MainWindow::addOrcTextTask(QPixmap pixmap){
    // 创建一个任务并将其添加到线程池
    taskCount += 1;
    qDebug()<<"add taskCount:"<<taskCount;
    noteLabel->setText("正在识别...");
    OcrTextTask* task = new OcrTextTask(pixmap);
    task->setAutoDelete(true);
    connect(task, SIGNAL(finished(QString)), this, SLOT(onRunnableFinished(QString)));
    threadPool->start(task);
}

void MainWindow::receiveCaptureCancel(){
    this->setWindowOpacity(1);
    //this->show();
}
void MainWindow::receiveScreenCapture(QPixmap catureImage){
    if(!scene->items().isEmpty()){
        QGraphicsItem *oldImage = scene->items().first();
        scene->removeItem(oldImage);
        view->resetMouseState();
        delete oldImage;
    }
    scene->addPixmap(catureImage);
    this->setWindowOpacity(1);
    addOrcTextTask(catureImage);
}
MainWindow::~MainWindow()
{
}

