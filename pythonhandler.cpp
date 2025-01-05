#include "pythonhandler.h"

QString PythonHandler::pythonHome = "D:/User/Python/Python312";
    // "D:/User/Qt/QtProject/FutureOCR/python/python-3.12.3-embed-amd64";

    // "D:/User/Python/Python312";
// python调用源代码坐在目录
QString PythonHandler::pyhtonSourceDir = "D:/User/Python/PycharmProject/ocr/developer";

bool PythonHandler::init(){
    if(Py_IsInitialized()==1){
        return true;
    }
    qDebug()<<"python解释器路径(pythonHome)："<<pythonHome;
    qDebug()<<"python文件位置(targetSourceDir)："<<pyhtonSourceDir;
    // 设置 Python 路径
    // try{
    const char* pythonPath = pythonHome.toUtf8().constData();
    std::wstring widePythonPath(pythonPath, pythonPath + strlen(pythonPath));
    Py_SetPythonHome(widePythonPath.c_str());
    // 初始化pyhton解释器
    Py_Initialize();
    // 判断是否已初始化
    if (Py_IsInitialized() != 0) {
        qDebug()<<"python解释器初始化成功.";
        // 使用sys模块加载python源文件
        PyRun_SimpleString("import sys");
        // python源文件所在文件夹
        // PyRun_SimpleString("sys.path.append('D:/User/Python/PycharmProject/ocr/developer')");
        // QCoreApplication::applicationDirPath()
        QString targetSourceDir = QString("sys.path.append('%1')").arg(pyhtonSourceDir);
        PyRun_SimpleString(targetSourceDir.toStdString().c_str());
        // 初始化线程支持
        PyEval_InitThreads();
        // 启动子线程前执行，为了释放PyEval_InitThreads获得的全局锁，否则子线程可能无法获取到全局锁
        PyEval_ReleaseThread(PyThreadState_Get());
        // isInit = true;
        return true;
    } else {
        qDebug()<<"Python解释器初始化失败.";
        if(PyErr_Occurred()) {
            PyErr_Print();
            // 清除错误状态
            PyErr_Clear();
        }
        // isInit = false;
        return false;
    }
    // }catch(const char* error){
    //     qDebug()<<"加载python解释器异常："<<error;
    //     return false;
    // }

}

void PythonHandler::close() {
    if(Py_IsInitialized()==1){
        qDebug()<<"调用 Py_Finalize.";
        Py_Finalize();
    }
}

void PythonHandler::loadIni(){
    // 创建 QSettings 对象并指定属性文件路径
    QString appDir = QCoreApplication::applicationDirPath();
    QString pythonConf = "pythonconf.ini";
    qDebug()<<"appDir:"<<appDir;
    QSettings settings(appDir+"/"+pythonConf, QSettings::IniFormat);
    // 读取属性值
    QString pyHome = settings.value("python_home").toString();
    if(pyHome==""){
        qDebug()<<"pythonconf.ini 未配置pyhton解释器路径(pyhtonHome),将使用默认值："<<pythonHome;
    }else{
        qDebug()<<"使用 pythonconf.ini 配置的解释器路径："<<pyHome;
        pythonHome = pyHome;
    }
    QString pySourceDir = settings.value("python_source_dir").toString();
    if(pySourceDir==""){
        qDebug()<<"pythonconf.ini 未配置pyhton执行源码目录(pyhtonSourceDir),将使用默认值："<<pyhtonSourceDir;
    }else{
        qDebug()<<"使用 pythonconf.ini 配置的pyhton源码目录(pyhtonSourceDir)："<<pySourceDir;
        pyhtonSourceDir = pySourceDir;
    }
}

void PythonHandler::setLocation(QString pyHome,QString pySourceDir){
    if(Py_IsInitialized()==1){
        qDebug()<<"python解释器已初始化，无法修改python配置.";
        return;
    }
    pythonHome = pyHome;
    pyhtonSourceDir = pySourceDir;
}

