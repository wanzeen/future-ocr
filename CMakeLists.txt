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
    set(PYTHON_HOME "D:/User/Qt/QtProject/FutureOCR/python/Python312")
    message(STATUS "Pyhon Debug..")
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(PYTHON_HOME "D:/User/Python/Python312")
    message(STATUS "Pyhon Release...")
    # 添加优化相关的配置
    add_definitions(-DOPTIMIZE_MODE)
endif()
# 添加调试相关的配置
add_definitions(-DDEBUG_MODE)
# 其他 Debug 模式下的配置
set(Python3_ROOT_DIR ${PYTHON_HOME})
# set(Python3_FIND_STRATEGY LOCATION)
# set(Python3_ROOT_DIR D:/User/Python/Python/Python311) # 修改为您的 Python3 安装路径
set(Python3_INCLUDE_DIRS ${PYTHON_HOME}/include)
# find_package(Python3 COMPONENTS Interpreter Development)
# set(Python3_DIR "D:/User/vcpkg/packages/python3_x64-windows/share/python3")
set(Python3_LIBRARIES ${PYTHON_HOME}/libs)
set(Python3_DLL_DIR ${PYTHON_HOME})
include_directories(${PYTHON_HOME}/include)
link_directories(${PYTHON_HOME}/libs)
find_package(Python3 COMPONENTS Development REQUIRED)


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

# set(BOOST_ROOT "D:/User/vcpkg/packages/boost-interprocess_x64-windows")  # 替换为 Boost 安装路径
# set(BOOST_ROOT "D:/User/vcpkg/packages")  # 替换为 Boost 安装路径
# set(Boost_DIR "")
# set(Boost_INCLUDE_DIR "D:/User/vcpkg/installed/x64-windows/include")
# 设置 Boost 的路径，如果你使用 vcpkg，直接引用 vcpkg 的安装路径
# set(Boost_INCLUDE_DIR "D:/path/to/boost/include")  # Boost 头文件路径
# set(Boost_LIBRARY_DIR "D:/User/vcpkg/installed/x64-windows/lib")  # Boost 库文件路径
# set(BOOST_ROOT "D:/User/vcpkg/installed/x64-windows")
# set(BOOST_LIBRARYDIR "D:/User/vcpkg/installed/x64-windows/lib")
# include_directories(${Boost_INCLUDE_DIRS})
# set(Boost_INCLUDE_DIR "D:/User/vcpkg/packages/boost-interprocess_x64-windows/include")
# set(BOOST_LIBRARYDIR "D:/User/vcpkg/packages/boost-interprocess_x64-windows/stage/lib")
# find_package(Boost REQUIRED COMPONENTS interprocess)
# set(BOOST_ROOT "D:/User/Qt/boost_1_87_0/boost")
# set(BOOST_INCLUDE_DIR "${BOOST_ROOT}")
# set(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib")
# set(BOOST_LIBRARYDIR "D:/User/vcpkg/installed/x64-windows/lib")
# find_package(Boost REQUIRED COMPONENTS system filesystem interprocess)
# include_directories("D:/User/vcpkg/installed/x64-windows/include")

# if (Boost_FOUND)
#     message(STATUS "Boost found: ${Boost_INCLUDE_DIRS}")
#     include_directories(${Boost_INCLUDE_DIRS})  # 包含 Boost 头文件目录
#     link_directories(${Boost_LIBRARY_DIR})  # 设置 Boost 库目录
# else()
#     message(FATAL_ERROR "Boost not found!")
# endif()

# find_package(Boost REQUIRED [COMPONENTS <libs>...])
# target_link_libraries(main PRIVATE Boost::boost Boost::<lib1> Boost::<lib2> ...)

# add_executable(shared_memory_example main.cpp)
# target_link_libraries(shared_memory_example Boost::Boost)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(FutureOCR
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        resources.qrc
        pythonhandler.h pythonhandler.cpp
        pythreadstatelock.h pythreadstatelock.cpp
        paddleocrtask.h paddleocrtask.cpp
        README.md
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

# file:///D:/User/vcpkg/packages/opencv4_x64-windows/debug/bin/opencv_highgui4d.dll
target_link_libraries(${PROJECT_NAME} PRIVATE Tesseract::libtesseract)

target_link_libraries(${PROJECT_NAME} PRIVATE leptonica)

# target_link_libraries(${PROJECT_NAME} PRIVATE Boost::boost Boost::interprocess)

# target_link_libraries(${PROJECT_NAME} PRIVATE pybind11::headers)

# target_link_libraries(${PROJECT_NAME} PRIVATE Python3::Python)
# target_include_directories(${PROJECT_NAME} PRIVATE ${Python3_INCLUDE_DIRS})
# target_link_libraries(${PROJECT_NAME} PRIVATE python3 python311)
# target_include_directories(${PROJECT_NAME} PRIVATE ${Python3_INCLUDE_DIR})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(${PROJECT_NAME} PRIVATE python312_d)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_libraries(${PROJECT_NAME} PRIVATE python312)
endif()

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
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
    # DEBUG
    COMMENT "Copying dll" VERBATIM
    COMMAND ${CMAKE_COMMAND} -E copy "${Python3_DLL_DIR}/python312.dll" "${CMAKE_BINARY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy "${Python3_DLL_DIR}/python3.dll" "${CMAKE_BINARY_DIR}"
    )
endif()
