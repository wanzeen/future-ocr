#include "pythonhandler.h"

bool PythonHandler::isInit = false;

PythonHandler::PythonHandler() {
}

bool PythonHandler::initPython(){
    if(isInit){
        return true;
    }
    // 设置 Python 路径
    const char* pythonPath = "D:/User/Python/Python312";
    std::wstring widePythonPath(pythonPath, pythonPath + strlen(pythonPath));
    Py_SetPythonHome(widePythonPath.c_str());
    // 初始化pyhton解释器
    Py_Initialize();
    // 判断是否已初始化
    if (Py_IsInitialized() != 0) {
        qDebug()<<"python解释器已初始化.";
    } else {
        qDebug()<<"Python解释器初始化失败.";
        if(PyErr_Occurred()) {
            PyErr_Print();
            // 清除错误状态
            PyErr_Clear();
        }
        return false;
    }
    // 使用sys模块加载python源文件
    PyRun_SimpleString("import sys");
    // python源文件所在文件夹
    PyRun_SimpleString("sys.path.append('D:/User/Python/PycharmProject/ocr')");
    // 初始化线程支持
    PyEval_InitThreads();
    // 启动子线程前执行，为了释放PyEval_InitThreads获得的全局锁，否则子线程可能无法获取到全局锁
    PyEval_ReleaseThread(PyThreadState_Get());
    return true;
}
PythonHandler::~PythonHandler() {
    qDebug()<<"释放Python解释器.";
    if(isInit){
     Py_Finalize();
    }
}

