#include "mainwindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include "screencapture.h"
#include <QFuture>
#include <QShortcut>
#include <QRadioButton>
#include <QCheckBox>
#include <QJsonDocument>
#include <QScrollBar>
#include "winhookwidget.h"
#include "globalsignals.h"
#include "imagetool.h"
#include "pythonhandler.h"
#include <QSplitter>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{


    qDebug()<<"初始化主窗体.";
    this->setWindowTitle("FutureOCR");
    this->loadPreferenceInfo();
    // 创建一个QIcon对象，用于表示应用程序图标
    QIcon icon(":/icons/FutureOCR.ico");

    // 设置应用程序窗口的图标
    this->setWindowIcon(icon);

    QWidget *centralWidget = new QWidget(this);
    // 窗口布局
    centralLayout = new QVBoxLayout();
    // 键盘事件
    initGlobalKeyEvent();
    // 识别完成事件
    GlobalSignals *instance =  GlobalSignals::instance();
    connect(instance,&GlobalSignals::signalMessageResult,this,&MainWindow::receiveMessageResult);

    // 中央组件
    centralWidget->setLayout(centralLayout);
    this->setCentralWidget(centralWidget);

    // 避免重复使用时无法通过if判断出现错误
    captureScreen = nullptr;
    initMenuTool();
    initImageView();
    initTextView();

    // 设置线程池的最大线程数
    threadPool = new QThreadPool();
    threadPool->setMaxThreadCount(MAX_THREAD_COUNT);
    qDebug()<<"创建线程池，最大线程数量："<<MAX_THREAD_COUNT;
}



void MainWindow::initGlobalKeyEvent(){
    qDebug()<<"初始化全局键盘监听事件.";
#ifdef Q_OS_WIN
    WinHookWidget *hook = new WinHookWidget();
    centralLayout->addWidget(hook);
    GlobalSignals *instance =  GlobalSignals::instance();
    connect(instance,&GlobalSignals::globalSignalKeyEvent,this,&MainWindow::receiveGlobalSignalKeyEvent);
#endif
}


void MainWindow::receiveGlobalSignalKeyEvent(int eventType){
    if(eventType==1){
        onCaptureButtonClicked();
    }
}

QSize MainWindow::getSize(){
    //QFont font("Arial", 12);
    QFontMetrics fontMetrics(this->font());
    //int charWidth = fontMetrics.horizontalAdvance('H');
    //qDebug()<<"charWidht:"<<charWidth;
    QSize size = fontMetrics.size(Qt::TextSingleLine,"汉");
    //qDebug()<<"size--w:"<<size.width()<<", h:"<<size.height();
    int areaWidth = size.width() * 20;
    int areaHeight = size.height() * 10;
    return QSize(areaWidth,areaHeight);
    //qDebug()<<"areaSize:"<<areaWidth<<", h:"<<areaHeight;
}

void MainWindow::initMenuTool(){
    qDebug()<<"初始化窗体菜单.";
    toolBar = new QToolBar();
    // captureAction = new QAction("截屏(Prt)");
    ocrAction = new QAction("文本提取(En)");

    pasteImageAction = new QAction("粘贴图片(V)");

    openAction = new QAction("打开(O)");

    copyImageAction = new QAction("复制图片(Shi+C)");
    saveAction = new QAction("保存(S)");
    preferenceAction = new QAction("设置");
    aboutAction = new QAction("关于");
    // toolBar->addAction(captureAction);
    toolBar->addAction(ocrAction);
    toolBar->addAction(pasteImageAction);
    toolBar->addAction(openAction);
    toolBar->addAction(copyImageAction);
    toolBar->addAction(saveAction);
    toolBar->addAction(preferenceAction);
    toolBar->addAction(aboutAction);

    //添加工具栏到窗口顶部
    this->addToolBar(Qt::TopToolBarArea,toolBar);
    //绑定事件
    // QObject::connect(captureAction, &QAction::triggered, this, &MainWindow::onCaptureButtonClicked);
    // 粘贴图片
    QObject::connect(pasteImageAction, &QAction::triggered, this, &MainWindow::onPasteImageClicked);
    QObject::connect(ocrAction, &QAction::triggered, this, &MainWindow::onPixmapOcrClicked);
    QObject::connect(openAction, &QAction::triggered, this, &MainWindow::onOpenButtonClicked);
    QObject::connect(saveAction, &QAction::triggered, this, &MainWindow::onSaveButtonClicked);
    QObject::connect(copyImageAction, &QAction::triggered, this, &MainWindow::onCopyImageClicked);
    QObject::connect(preferenceAction, &QAction::triggered, this, &MainWindow::onPreferenceClicked);
    QObject::connect(aboutAction, &QAction::triggered, this, &MainWindow::onAboutClicked);
}

void MainWindow::initImageView(){
    qDebug()<<"初始化图像显示区.";
    // 创建一个 QGraphicsView 对象
    imageView = new CustomGraphicsView();

    imageView->setMinimumSize(getSize());

    QHBoxLayout *captureLayout = new QHBoxLayout();
    captureLayout->addWidget(imageView);
    //添加：图片显示区
    centralLayout->addLayout(captureLayout);
    //消息传递：图片选中
    connect(imageView,&CustomGraphicsView::signalGraphicsViewSelected,this,&MainWindow::receiveGraphicsViewSelected);
    connect(imageView,&CustomGraphicsView::signalGraphicsPixmapItemAdded,this,&MainWindow::receiveGraphicsPixmapItemAdded);
    connect(imageView,&CustomGraphicsView::signalGraphicsPixmapChangedDescription,this,&MainWindow::receivePixmapChangedDescription);
}
void MainWindow::receivePixmapChangedDescription(QString description){
    noteLabel->setText(description);
}

void MainWindow::onPreferenceClicked(){
    tempPreferenceInfo.captureAutoOcr = preferenceInfo.captureAutoOcr;
    tempPreferenceInfo.pasteImageAutoOcr = preferenceInfo.pasteImageAutoOcr;
    tempPreferenceInfo.copyTextAfterAutoMiniWindow = preferenceInfo.copyTextAfterAutoMiniWindow;
    tempPreferenceInfo.enableImageEnhance = preferenceInfo.enableImageEnhance;
    // 创建一个QDialog
    QDialog dialog(this);
    dialog.setWindowTitle("偏好设置");

    QVBoxLayout dialogLayout;

    QHBoxLayout captureAfterLayout;
    QFontMetrics fontMetrics(this->font());
    QSize size = fontMetrics.size(Qt::TextSingleLine,"汉");

    int lableWidth = size.width() * 10;


    // 创建一个QLabel
    QLabel captureSettingLabel("截屏选中后");
    captureSettingLabel.setAlignment(Qt::AlignLeft);
    captureSettingLabel.setMinimumWidth(lableWidth);
    QCheckBox autoOcrCheck("立即识别");
    autoOcrCheck.setProperty("id",1);
    captureAfterLayout.addWidget(&captureSettingLabel);
    captureAfterLayout.addWidget(&autoOcrCheck);
    dialogLayout.addLayout(&captureAfterLayout);
    autoOcrCheck.setChecked(preferenceInfo.captureAutoOcr==1);


    QHBoxLayout pasteImageLayout;
    QLabel pasteImageLabel("粘贴图片后");
    pasteImageLabel.setAlignment(Qt::AlignLeft);
    pasteImageLabel.setMinimumWidth(lableWidth);
    QCheckBox autoOcrImageCheck("立即识别");
    autoOcrImageCheck.setProperty("id",2);
    pasteImageLayout.addWidget(&pasteImageLabel);
    pasteImageLayout.addWidget(&autoOcrImageCheck);
    dialogLayout.addLayout(&pasteImageLayout);
    autoOcrImageCheck.setChecked(preferenceInfo.pasteImageAutoOcr==1);


    QHBoxLayout coptTextLayout;
    QLabel copyTextLabel("复制文本后");
    copyTextLabel.setAlignment(Qt::AlignLeft);
    copyTextLabel.setMinimumWidth(lableWidth);
    QCheckBox autoMiniWindowCheck("最小化窗口");
    autoMiniWindowCheck.setProperty("id",3);
    coptTextLayout.addWidget(&copyTextLabel);
    coptTextLayout.addWidget(&autoMiniWindowCheck);
    autoMiniWindowCheck.setChecked(preferenceInfo.copyTextAfterAutoMiniWindow==1);
    dialogLayout.addLayout(&coptTextLayout);

    QHBoxLayout enhanceImageLayout;
    QLabel enhanceImageLabel("OCR图像增强");
    enhanceImageLabel.setAlignment(Qt::AlignLeft);
    enhanceImageLabel.setMinimumWidth(lableWidth);
    QCheckBox enchanceImageCheck("启用");
    enchanceImageCheck.setProperty("id",4);
    enhanceImageLayout.addWidget(&enhanceImageLabel);
    enhanceImageLayout.addWidget(&enchanceImageCheck);
    enchanceImageCheck.setChecked(preferenceInfo.enableImageEnhance==1);
    dialogLayout.addLayout(&enhanceImageLayout);
    QLabel enchanceDescription("提升图片中包含5号及以上较小字体识别准确率，但会使OCR速度降低");
    QFont font;
    //font.setItalic(true);
    font.setPointSize(12);
    QPalette palette = enchanceDescription.palette();
    // 设置前景色为浅黑色（RGB: 86, 86, 86）
    palette.setColor(QPalette::WindowText, QColor(86, 86, 86));
    enchanceDescription.setPalette(palette);
    enchanceDescription.setFont(font);
    dialogLayout.addWidget(&enchanceDescription);

    QHBoxLayout operateLayout;
    QPushButton savePreferenceBtn("保存");
    QPushButton cancelPreferenceBtn("取消");
    operateLayout.addWidget(&savePreferenceBtn);
    operateLayout.addWidget(&cancelPreferenceBtn);
    dialogLayout.addLayout(&operateLayout);

    dialog.setLayout(&dialogLayout);

    // 信号槽连接
    connect(&autoOcrCheck, &QCheckBox::stateChanged, this, &MainWindow::receivePreferenceCheckBoxChanged);
    connect(&autoOcrImageCheck, &QCheckBox::stateChanged, this, &MainWindow::receivePreferenceCheckBoxChanged);
    connect(&autoMiniWindowCheck, &QCheckBox::stateChanged, this, &MainWindow::receivePreferenceCheckBoxChanged);
    connect(&enchanceImageCheck, &QCheckBox::stateChanged, this, &MainWindow::receivePreferenceCheckBoxChanged);


    // 信号槽连接
    connect(&savePreferenceBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(&cancelPreferenceBtn, &QPushButton::clicked, &dialog, &QDialog::reject);
    // 显示对话框
    int result = dialog.exec();
    //
    if (result == QDialog::Accepted) {
        // 用户点击了确认按钮
        // 执行保存操作
        qDebug()<<"保存设置.";
        preferenceInfo.captureAutoOcr = tempPreferenceInfo.captureAutoOcr;
        preferenceInfo.pasteImageAutoOcr = tempPreferenceInfo.pasteImageAutoOcr;
        preferenceInfo.copyTextAfterAutoMiniWindow = tempPreferenceInfo.copyTextAfterAutoMiniWindow;
        preferenceInfo.enableImageEnhance = tempPreferenceInfo.enableImageEnhance;
        savePreferenceSetting();
    } else if (result == QDialog::Rejected) {
        // 用户点击了取消按钮
        // 执行取消操作
        qDebug()<<"取消设置.";
    }
}
void MainWindow::receivePreferenceCheckBoxChanged(int state){
    // 获取发送信号的复选框对象
    QCheckBox *senderCheckBox = qobject_cast<QCheckBox*>(sender());
    if(senderCheckBox->property("id").toInt()==1){
        tempPreferenceInfo.captureAutoOcr = state?1:0;
    }else if(senderCheckBox->property("id").toInt()==2){
        tempPreferenceInfo.pasteImageAutoOcr = state?1:0;
    }else if(senderCheckBox->property("id").toInt()==3){
        tempPreferenceInfo.copyTextAfterAutoMiniWindow = state?1:0;
    }else if(senderCheckBox->property("id").toInt()==4){
        tempPreferenceInfo.enableImageEnhance = state?1:0;
    }
}

void MainWindow::loadPreferenceInfo(){
    //定义文件对象
    QFile file(getConfigPath());
    qDebug()<<"访问设置文件："<<getConfigPath();
    //打开文件，读写文件
    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QTextStream inStream(&file);
        QString jsonContent = inStream.readAll();
        qDebug()<<"加载偏好设置内容："<<jsonContent;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonContent.toUtf8());
        //QJsonDocument 转为 QJsonObject
        QJsonObject jsonObj = jsonDoc.object();
        if(!jsonObj.value("preferenceInfo").isNull()){
            QJsonObject preferenceObj = jsonObj.value("preferenceInfo").toObject();
            if(!preferenceObj.value("captureAutoOcr").isNull()){
                preferenceInfo.captureAutoOcr = preferenceObj.value("captureAutoOcr").toInt(0);
            }
            if(!preferenceObj.value("captureAutoOcr").isNull()){
                preferenceInfo.pasteImageAutoOcr = preferenceObj.value("pasteImageAutoOcr").toInt(0);
            }
            if(!preferenceObj.value("copyTextAfterAutoMiniWindow").isNull()){
                preferenceInfo.copyTextAfterAutoMiniWindow = preferenceObj.value("copyTextAfterAutoMiniWindow").toInt(0);
            }
            if(!preferenceObj.value("enableImageEnhance").isNull()){
                preferenceInfo.enableImageEnhance = preferenceObj.value("enableImageEnhance").toInt(0);
            }
            // qDebug()<<"load data：captureAutoOcr="<<preferenceInfo.captureAutoOcr<<", pasteImageAuroOcr="<<preferenceInfo.pasteImageAutoOcr;
        }
        file.close();
        return;
    }
    qDebug()<<"初始化加载设置数据失败，无法打开文件.";

}
QString MainWindow::getConfigPath(){
    return  QCoreApplication::applicationDirPath()+"/conf.json";
}

bool MainWindow::savePreferenceSetting(){
    //qDebug()<<"save data：captureAutoOcr="<<preferenceInfo.captureAutoOcr<<", pasteImageAuroOcr="<<preferenceInfo.pasteImageAutoOcr;
    // 获取文件路径的目录部分
    QString filePath = getConfigPath();
    QFileInfo fileInfo(filePath);
    QString directoryPath = fileInfo.path();
    QFile file(filePath);
    //qDebug()<<"filePath:"<<filePath;
    // 判断目录是否存在，如果不存在则创建
    QDir directory;
    if (!directory.exists(directoryPath)) {
        if (directory.mkpath(directoryPath)) {
            qDebug() << "成功创建设置文件: " << directoryPath;
        } else {
            qDebug() << "创建设置文件失败: " << directoryPath;
            return false;
        }
    }
    if (file.open(QIODevice::WriteOnly)) {
        QJsonObject jsonObject;
        QJsonObject objectItem;
        objectItem["captureAutoOcr"] = preferenceInfo.captureAutoOcr;
        objectItem["pasteImageAutoOcr"] = preferenceInfo.pasteImageAutoOcr;
        objectItem["copyTextAfterAutoMiniWindow"] = preferenceInfo.copyTextAfterAutoMiniWindow;
        objectItem["enableImageEnhance"] = preferenceInfo.enableImageEnhance;
        jsonObject["preferenceInfo"] = objectItem;
        QJsonDocument jsonDocument(jsonObject);
        QString json = jsonDocument.toJson(QJsonDocument::Indented);
        QTextStream out(&file);
        out << json;
        file.close();
        qDebug()<<"设置内容已保存："<<json;
        return true;
    } else {
        qDebug() << "无法打开设置配置文件!";
    }
    return false;
}




// 文本显示区初始化
void MainWindow::initTextView(){
    edit = new QPlainTextEdit();
    //设置：文本框尺寸
    edit->setMinimumSize(getSize());
    edit->setPlaceholderText("识别内容");
    // 添加：文本显示框
    centralLayout->addWidget(edit);

    copyTextBtn = new QPushButton("复制已选文本");
    captureBtn = new QPushButton("截屏(PrtSc)");
    QHBoxLayout *textOperateLayout = new QHBoxLayout();
    textOperateLayout->addWidget(captureBtn);
    textOperateLayout->addWidget(copyTextBtn);
    // 添加：操作按钮
    centralLayout->addLayout(textOperateLayout);
    copyTextBtn->setEnabled(false);

    noteLabel = new QLabel("");
    noteLabel->setAlignment(Qt::AlignCenter);

    QFont font;
    font.setPointSize(12);
    //添加：底部状态通知
    noteLabel->setFont(font);
    noteLabel->setWordWrap(true);
    // 设置文本可被鼠标选中
    noteLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    centralLayout->addWidget(noteLabel);
    // 创建调色板对象
    QPalette palette;
    // 设置前景色为浅褐色
    palette.setColor(QPalette::WindowText, QColor(51, 51, 51));
    // 应用调色板到QLabel
    noteLabel->setPalette(palette);
    //事件：文本选中状态改变
    connect(edit, &QPlainTextEdit::selectionChanged, this, &MainWindow::handleTextSelectionChanged);
    // 文本改变
    connect(edit, &QPlainTextEdit::textChanged, this, &MainWindow::handleTextChanged);
    //事件：复制文本按钮
    QObject::connect(copyTextBtn, &QPushButton::clicked, this, &MainWindow::onCopyTextClicked);
    //事件：截屏按钮
    //QObject::connect(captureBtn, &QPushButton::clicked, this, &MainWindow::onPasteImageClicked);
    QObject::connect(captureBtn, &QPushButton::clicked, this, &MainWindow::onCaptureButtonClicked);
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


void MainWindow::onPasteImageClicked(){
    QClipboard *clipboard = QApplication::clipboard();
    QString text = clipboard->text();
    qDebug()<<"收到粘贴数据:"<<text;
    const QMimeData *mimeData = clipboard->mimeData();
    QPixmap pixmap;
    if (mimeData->hasImage()) {
        // if (variant.canConvert<QImage>()) {
        //     QImage image = variant.value<QImage>();
        // }
        pixmap = qvariant_cast<QPixmap>(mimeData->imageData());
        qDebug()<<"Ctr + V 图片数据大小："<<pixmap.width()<<" x "<<pixmap.height();

    }
    if(text.startsWith("file:///")){
        QString filePath = text.remove("file:///");
        qDebug()<<"Ctr+V粘贴文件:"<<filePath;
        QFileInfo fileInfo(filePath);
        // 判断文件是否为图片，你可以根据需要使用不同的方法
        QStringList imageExtensions;
        imageExtensions << "png" << "jpg" << "jpeg" << "gif" << "bmp";
        QString extension = fileInfo.suffix().toLower();
        if(imageExtensions.contains(extension)){
            pixmap = QPixmap(filePath);
        }
    }
    if(!pixmap.isNull()){
        QString message = QString("粘贴图像分辨率：%1 x %2").arg(pixmap.width()).arg(pixmap.height());
        qDebug()<<message;
        noteLabel->setText(message);
        QImage image = pixmap.toImage();
        imageView->setSrcImage(image);
        imageView->updatePixmap(pixmap);
        // 判断粘贴后是否自动识别
        if(preferenceInfo.pasteImageAutoOcr == 1){
            addOrcTextTask(pixmap);
            return;
        }
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
    // 设置窗口初始状态为最小化
    this->setWindowState(Qt::WindowMinimized);
}
void MainWindow::handleTextSelectionChanged(){
    if(edit->textCursor().hasSelection()){
        copyTextBtn->setText("复制已选文本");
    }else{
        copyTextBtn->setText("复制文本");
    }
}
void MainWindow::receiveMessageResult(QString content,int code) {
    qDebug()<<"已收到消息，消息编码："<<code;
    taskCount -= 1;
    if(code==MessageCode::OCR_OK){
        QString result = ocrTextFormat(content);
        QString message = "";
        if(result!=""){
            edit->setPlainText(result);
        }else{
            message = "，但未识别出内容.";
        }
        if(taskCount==0){
            noteLabel->setText("识别完成"+message);
        }else{
            noteLabel->setText("已识别1个"+message);
        }
    }else if(code==MessageCode::OCR_ERR){
        noteLabel->setText(content);
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

void MainWindow::receiveGraphicsPixmapItemAdded(){
    QString fileName = imageView->property("fileName").toString();
    noteLabel->setText("已加载图片："+fileName);
    if(preferenceInfo.pasteImageAutoOcr==1){
        onPixmapOcrClicked();
    }
}

// void MainWindow::hideEvent(QHideEvent *event){
//     // QMainWindow::hideEvent(event);
//     qDebug()<<"hideEvent...";
// }
void MainWindow::onAboutClicked(){
    // 创建一个QDialog
    QDialog dialog(this);
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
    content +="版本：1.0.0\n";
    content +="介绍：FutureOCR是一个一个光学文本识别工具，支持识别图像中的中英文字符，支持图像区域选中识别，识别功能采用多线程，具有较快的响应速度\n"
               "    该软件由C++语言开发，在最新开源库tesseract的基础上研发，引入了基于神经网络（LSTM） 的 OCR 引擎，对于标准中英文字体识别准确度较高，而对于中文斜体字的识别度欠佳；该软件内置中英文训练数据的支持而不依赖于网络，本地化的处理利于保护敏感数据安全\n";
    content += "提示：\n";
    content += "    图像显示区【Ctrl】+【Shift】+【鼠标滚轮】组合键，可实现图片旋转，旋转正的图片再识别具有较好的识别效果\n";
    content += "    图像显示区【Ctrl】+【鼠标滚轮】组合键，可实现图片放大缩小\n";
    content += "    图像显示区【Ctrl】+【V】组合键，可实现图片直接粘贴\n";
    content += "    系统快捷键【Ctrl】+【PrtSc】组合键，可实现立即截屏\n";
    content += "    【设置】中可依据个人使用习惯自定义截屏、粘贴图片、复制文本后的操作行为\n";
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
        QPixmap pixmap(filePath);
        QImage image = pixmap.toImage();
        imageView->setSrcImage(image);
        imageView->updatePixmap(pixmap);
        this->setProperty("path",filePath);
        QFileInfo fileInfo(filePath);
        noteLabel->setText("已打开图片："+fileInfo.fileName());
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
    QPixmap pixmap = imageView->getCurrPixmap();
    if(!pixmap.isNull()){
        // 打开文件选择器
        QString filePath = QFileDialog::getSaveFileName(this, "保存为...", "", "图片文件 (*.png *.jpg *.jpeg)");
        if (filePath != "") {
            // 将图片保存到指定的文件路径
            pixmap.save(filePath);
            noteLabel->setText("图片保存到："+filePath);
        }
    }else{
        noteLabel->setText("没有可保存的图片.");
    }
}

void MainWindow::onCopyImageClicked(){
    QPixmap pixmap = imageView->getCurrPixmap();
    if(!pixmap.isNull()){
        QImage currImage = pixmap.toImage();
        // 加载图片到QImage对象
        //QImage image("path/to/image.png");
        // 获取系统剪贴板
        QClipboard *clipboard = QApplication::clipboard();
        // 将图片复制到剪贴板
        clipboard->setImage(currImage);
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
    this->hide();
    if(captureScreen==nullptr){
        captureScreen = new ScreenCapture();
        //惊天bug，此处必须判断，不然出现重复绑定导致多次执行异常
        connect(captureScreen,&ScreenCapture::signalCaptureFinished,this, &MainWindow::receiveScreenCapture);
        connect(captureScreen,&ScreenCapture::signalCaptureCancel,this,&MainWindow::receiveCaptureCancel);
    }
    captureScreen->initScreenPixmap();
    captureScreen->setCapturePreference(preferenceInfo.captureAutoOcr==1);
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
    //qDebug()<<"按下键盘："<<event->key();
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_O) {
        onOpenButtonClicked();
    }else if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_S) {
        onSaveButtonClicked();
    }else if (event->modifiers() & Qt::ControlModifier && (event->modifiers() & Qt::ShiftModifier) && event->key() == Qt::Key_C) {
        onCopyImageClicked();
    }else if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_V) {
        onPasteImageClicked();
    }else if(event->modifiers() == Qt::ControlModifier && ( event->key() == Qt::Key_Return ||  event->key() ==Qt::Key::Key_Enter)){
        onPixmapOcrClicked();
    }else{
        QMainWindow::keyPressEvent(event);
    }
}


void MainWindow::onPixmapOcrClicked(){
    QPixmap pixmap = this->imageView->getCurrPixmap();
    if(!pixmap.isNull()){
        addOrcTextTask(pixmap);
    }else{
        noteLabel->setText("未读取到图片.");
    }
}


void MainWindow::addOrcTextTask(QPixmap pixmap){
    bool enablePaddleOcr = true;
    if(enablePaddleOcr){
        // 创建一个任务并将其添加到线程池
        taskCount += 1;
        //pixmap = adjustPixmap(pixmap);
        noteLabel->setText("正在识别...");
        // QPixmap targetPixmap = this->getAdjustedPixmap(pixmap);
        QImage image = pixmap.toImage();
        // ImageTool::showImage(image,"Adjust Image");
        // pixmap = QPixmap::fromImage(imageTarget);
        // Pix *pix = ImageTool::qImageToPix(image);
        PaddleOcrTask* task = new PaddleOcrTask(image);
        task->setAutoDelete(true);
        //connect(task, SIGNAL(finished(QString)), this, SLOT(receiveMessageResult(QString)));
        threadPool->start(task);
    }else{
    if(pixmap.width()>0 && pixmap.height()>0){
        // 创建一个任务并将其添加到线程池
        taskCount += 1;
        //pixmap = adjustPixmap(pixmap);
        noteLabel->setText("正在识别...");
        QPixmap targetPixmap = this->getAdjustedPixmap(pixmap);
        QImage image = targetPixmap.toImage();
        // ImageTool::showImage(image,"Adjust Image");
        // pixmap = QPixmap::fromImage(imageTarget);
        Pix *pix = ImageTool::qImageToPix(image);
        OcrTextTask* task = new OcrTextTask(pix);
        task->setAutoDelete(true);
        //connect(task, SIGNAL(finished(QString)), this, SLOT(receiveMessageResult(QString)));
        threadPool->start(task);
    }
    }

}

QPixmap MainWindow::getAdjustedPixmap(QPixmap &pixmap){
    // 定义放大倍数，对5号及以上字体提升识别效果
    QImage targetImage = pixmap.toImage();
    if(this->preferenceInfo.enableImageEnhance==1){
        float scaleFactor = 1.5;
        // 计算目标宽度和高度
        QSize targetSize(targetImage.width() * scaleFactor, targetImage.height() * scaleFactor);
        targetImage = targetImage.scaled(targetSize, Qt::KeepAspectRatio,Qt::SmoothTransformation);
    }
    // 转换为灰度图像
    QImage grayImage = targetImage.convertToFormat(QImage::Format_Grayscale8);
    return  QPixmap::fromImage(grayImage);
}
void MainWindow::receiveCaptureCancel(){
    this->setWindowOpacity(1);
    this->showNormal();
}
void MainWindow::receiveScreenCapture(QPixmap caturePixmap){
    qDebug()<<"收到截屏图片大小："<<caturePixmap.size();
    QImage image = caturePixmap.toImage();
    //QImage imageTarget = ImageTool::toGrayBinaryImage(image);
    imageView->setSrcImage(image);
    imageView->updatePixmap(caturePixmap);
    this->setWindowOpacity(1);
    // 最小化窗口
    // 确保最小化窗口正常显示出来
    this->showNormal();
    addOrcTextTask(caturePixmap);
}

MainWindow::~MainWindow()
{
}

