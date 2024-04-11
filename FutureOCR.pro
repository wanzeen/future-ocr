QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    capturescreen.cpp \
    main.cpp \
    mainwindow.cpp \
    ocrtexttask.cpp \
    screencapture.cpp

HEADERS += \
    basicdemo.h \
    capturescreen.h \
    mainwindow.h \
    ocrtexttask.h \
    screencapture.h

TRANSLATIONS += \
    FutureOCR_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += D:/User/vcpkg/packages/tesseract_x64-windows/include
LIBS += -LD:/User/vcpkg/packages/tesseract_x64-windows/lib/ -ltesseract53


INCLUDEPATH += D:/User/vcpkg/packages/leptonica_x64-windows/include
LIBS += -LD:/User/vcpkg/packages/leptonica_x64-windows/lib/ -lleptonica-1.84.1

