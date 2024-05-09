cmake_minimum_required(VERSION 3.5)

project(FutureOCR VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

#################### 链接 Python库  #####################
# 设置编译器标志
# set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
# set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Debug 构建类型下的配置
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Pyhon Debug..")
    # 添加调试相关的配置
    add_definitions(-DDEBUG_MODE)
    # 其他 Debug 模式下的配置
    set(Python3_ROOT_DIR D:/User/Python/Python312)
    # set(Python3_FIND_STRATEGY LOCATION)
    # set(Python3_ROOT_DIR D:/User/Python/Python/Python311) # 修改为您的 Python3 安装路径
    set(Python3_INCLUDE_DIRS D:/User/Python/Python312/include)
    # find_package(Python3 COMPONENTS Interpreter Development)
    # set(Python3_DIR "D:/User/vcpkg/packages/python3_x64-windows/share/python3")
    set(Python3_LIBRARIES D:/User/Python/Python312/libs)
    set(Python3_DLL_DIR D:/User/Python/Python312)
    include_directories(D:/User/Python/Python312/include)
    link_directories(D:/User/Python/Python312/libs)
    find_package(Python3 COMPONENTS Development REQUIRED)

endif()

# Release 构建类型下的配置
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Pyhon Release...")
    # 添加优化相关的配置
    add_definitions(-DOPTIMIZE_MODE)
    # 其他 Release 模式下的配置
    set(Python3_ROOT_DIR D:/User/Python/Python312)
    # set(Python3_FIND_STRATEGY LOCATION)
    # set(Python3_ROOT_DIR D:/User/Python/Python/Python311) # 修改为您的 Python3 安装路径
    set(Python3_INCLUDE_DIRS D:/User/Python/Python312/include)
    # find_package(Python3 COMPONENTS Interpreter Development)
    # set(Python3_DIR "D:/User/vcpkg/packages/python3_x64-windows/share/python3")
    set(Python3_LIBRARIES D:/User/Python/Python312/libs)
    set(Python3_DLL_DIR D:/User/Python/Python312)
    include_directories(D:/User/Python/Python312/include)
    link_directories(D:/User/Python/Python312/libs)
    find_package(Python3 COMPONENTS Development REQUIRED)
endif()



# set(PYTHON_LIBRARY "D:/User/Python/Python312/libs") # 根据您的 Python 版本和实际路径修改
# 添加 xxx.dll 文件
# add_library(python312 SHARED IMPORTED)

# 设置 xxx.dll 文件的路径
# set_target_properties(python312 PROPERTIES IMPORTED_LOCATION D:/User/Python/Python312/python312.dll)
# Python3_LIBRARIES Python3_INCLUDE_DIRS

# 如果使用的是非系统目录下的 Python 可以通过指定 Python3_ROOT_DIR 改变查找路径
# set(Python3_ROOT_DIR "${CMAKE_SOURCE_DIR}/venv")
# find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
# add_executable(cpp-python-test main.cpp)

# set(Python3_ROOT_DIR "D:/User/DevelopKit/Python/venv")
# set(Python3_DIR "D:/User/vcpkg/packages/python3_x64-windows/share/python3")
# set(Python3_INCLUDE_DIR "D:/User/vcpkg/packages/python3_x64-windows/include")
# set(Python3_LIBRARIE "D:/User/vcpkg/packages/python3_x64-windows/lib")


# find_package(Python3 COMPONENTS Development REQUIRED)
# find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
# find_package(Python COMPONENTS Interpreter Development)
# find_package(pybind11 CONFIG)

   # pybind11 method:
   # pybind11_add_module(MyModule1 src1.cpp)

   # Python method:
   # Python_add_library(MyModule2 src2.cpp)
   # set_target_properties(${PROJECT_NAME} PROPERTIES
   #     INTERPROCEDURAL_OPTIMIZATION ON
   #     CXX_VISIBILITY_PRESET ON
   #     VISIBILITY_INLINES_HIDDEN ON
   # )

#################### 链接 OpenCV库  #####################
set(OpenCV_DIR "D:/User/vcpkg/packages/opencv4_x64-windows/share/opencv4")
set(OPENCV_DLL_DIR "D:/User/vcpkg/packages/opencv4_x64-windows/bin")
include_directories("D:/User/vcpkg/packages/opencv4_x64-windows/include")

set(quirc_DIR "D:/User/vcpkg/installed/x64-windows/share/quirc")

set(Protobuf_DIR "D:/User/vcpkg/installed/x64-windows/share/protobuf")
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "building protobuf")


set(TIFF_DIR "D:/User/vcpkg/installed/x64-windows/share/tiff")
set(TIFF_INCLUDE_DIR "D:/User/vcpkg/packages/tiff_x64-windows")
set(TIFF_LIBRARY "D:/User/vcpkg/packages/tiff_x64-windows/lib/tiff.lib")
find_package(TIFF REQUIRED)
message(STATUS "building tiff")

find_package(OpenCV CONFIG REQUIRED)

link_directories(D:/User/vcpkg/packages/opencv4_x64-windows/lib)

#################### 链接 tesseract库 #####################
set(OpenJPEG_DIR "D:/User/vcpkg/installed/x64-windows/share/openjpeg")
find_package(OpenJPEG CONFIG REQUIRED)

set(WebP_DIR "D:/User/vcpkg/installed/x64-windows/share/WebP")

set(CURL_INCLUDE_DIR "D:/User/vcpkg/packages/curl_x64-windows/include")

set(LibArchive_INCLUDE_DIR "D:/User/vcpkg/packages/libarchive_x64-windows/include")
set(LibArchive_LIBRARY "D:/User/vcpkg/packages/libarchive_x64-windows/lib/archive.lib")

set(Leptonica_DIR "D:/User/vcpkg/installed/x64-windows/share/leptonica")
find_package(Leptonica CONFIG REQUIRED)
message(STATUS "building leptonica")

set(Tesseract_DIR "D:/User/vcpkg/installed/x64-windows/share/tesseract")
set(Tesseract_INCLUDE_DIR "D:/User/vcpkg/packages/tesseract_x64-windows/include")
find_package(Tesseract CONFIG REQUIRED)
message(STATUS "building tesseract")


set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        customgraphicsview.cpp
        customgraphicsview.h
        globalsignals.cpp
        globalsignals.h
        ocrtexttask.cpp
        ocrtexttask.h
        screencapture.cpp
        screencapture.h
        winhookwidget.cpp
        winhookwidget.h
        imagetool.h
        imagetool.cpp
        logspace.h
        logspace.cpp

)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(FutureOCR
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        resources.qrc
        pythonhandler.h pythonhandler.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET FutureOCR APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(FutureOCR SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(FutureOCR
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(FutureOCR PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

target_link_libraries(${PROJECT_NAME} PRIVATE opencv_ml opencv_dnn opencv_core opencv_flann opencv_imgcodecs)
# file:///D:/User/vcpkg/packages/opencv4_x64-windows/debug/bin/opencv_highgui4d.dll
target_link_libraries(${PROJECT_NAME} PRIVATE Tesseract::libtesseract)

target_link_libraries(${PROJECT_NAME} PRIVATE leptonica)

# target_link_libraries(${PROJECT_NAME} PRIVATE pybind11::headers)

# target_link_libraries(${PROJECT_NAME} PRIVATE Python3::Python)
# target_include_directories(${PROJECT_NAME} PRIVATE ${Python3_INCLUDE_DIRS})
# target_link_libraries(${PROJECT_NAME} PRIVATE python3 python311)
# target_include_directories(${PROJECT_NAME} PRIVATE ${Python3_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE python312)


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.FutureOCR)
endif()
set_target_properties(FutureOCR PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS FutureOCR
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(FutureOCR)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "copy Pyhton3 dll...")
    message(STATUS "save dll position: ${CMAKE_BINARY_DIR}")
    ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${Python3_DLL_DIR}/python312_d.dll" "${CMAKE_BINARY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy "${Python3_DLL_DIR}/python3_d.dll" "${CMAKE_BINARY_DIR}"
    )

    message(STATUS "copy OpenCV dll...")
    # opencv 复制dll文件到当前构建目录下
    ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${OPENCV_DLL_DIR}/opencv_core4.dll" "${CMAKE_BINARY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy "${OPENCV_DLL_DIR}/opencv_imgproc4.dll" "${CMAKE_BINARY_DIR}"
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
    # DEBUG
    COMMENT "Copying dll" VERBATIM
    COMMAND ${CMAKE_COMMAND} -E copy "${Python3_DLL_DIR}/python312.dll" "${CMAKE_BINARY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy "${Python3_DLL_DIR}/python3.dll" "${CMAKE_BINARY_DIR}"
    )
    ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${OPENCV_DLL_DIR}/opencv_core4.dll" "${CMAKE_BINARY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy "${OPENCV_DLL_DIR}/opencv_imgproc4.dll" "${CMAKE_BINARY_DIR}"
    )
endif()
