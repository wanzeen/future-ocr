#ifndef PYTHONRESOURCE_H
#define PYTHONRESOURCE_H

#include "pythonhandler.h"

class PythonResource
{
public:
    static PythonHandler* pyhtonHandler(){
        if(pyhtonHandler==nullptr){
            pythonHandler = new PythonHandler();
        }
        return pythonHandler;
    }
    static void clear(){
        if(pythonHandler!=nullptr){
            pythonHandler->close();
            delete pythonHandler;
        }
    }
private:
    static PythonHandler* pythonHandler;

};

#endif // PYTHONRESOURCE_H
