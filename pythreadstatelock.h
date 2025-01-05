
//将全局解释器锁和线程的相关操作用类封装
#ifndef PYTHREADSTATELOCK_H
#define PYTHREADSTATELOCK_H
#include <Python.h>


class PyThreadStateLock
{
public:
    PyThreadStateLock(void)
    {
        _save = nullptr;
        nStatus = 0;
        //检测当前线程是否拥有GIL
        nStatus = PyGILState_Check();
        PyGILState_STATE gstate;
        if (!nStatus)
        {
            //如果没有GIL，则申请获取GIL
            gstate = PyGILState_Ensure();
            nStatus = 1;
            qDebug()<<"获得GIL.";
        }
        _save = PyEval_SaveThread();
        PyEval_RestoreThread(_save);
    }
    ~PyThreadStateLock(void)
    {
        qDebug()<<"释放GIL.";
        _save = PyEval_SaveThread();
        PyEval_RestoreThread(_save);
        if (nStatus)
        {
            //释放当前线程的GIL
            PyGILState_Release(gstate);
        }

    }

private:
    PyGILState_STATE gstate;
    PyThreadState *_save;
    int nStatus;

};

#endif // PYTHREADSTATELOCK_H
