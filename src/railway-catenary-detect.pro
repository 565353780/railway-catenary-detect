QT += qml quick sql xml widgets network  webview webengine
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

DESTDIR= $$PWD/../bin/

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
    modules/statistics/statisticcontroller.cpp \
    singleapplication.cpp \
    modules/detect/objectdetect.cpp \
    modules/review/reviewcontroller.cpp \
    modules/railwayCatenaryDataManager/railwaycatenarydatamanager.cpp \
    services/HttpApi/HttpApi.cpp \
    services/HttpApi/MySocketClient.cpp \
    services/HttpApi/SimpleClient.cpp \
    services/HttpApi/SimpleServer.cpp \
    services/SqlTable/SqlRecordCountTableModel.cpp \
    services/filewatcher/compactfilesystemwatcher.cpp \
    services/filewatcher/copyfilethread.cpp \
    services/filewatcher/RemoveFileThread.cpp \
    services/DatabaseManager/databasemanager.cpp \
    modules/dataLoader/catenarypicdataloader.cpp \
    modules/dataLoader/reviewjsondataloader.cpp \
    modules/maincontroller.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    modules/statistics/statisticcontroller.h \
    modules/statistics/statisticcontroller.h \
    singleapplication.h \
    modules/detect/objectdetect.h \
    modules/review/reviewcontroller.h \
    modules/railwayCatenaryDataManager/railwaycatenarydatamanager.h \
    services/HttpApi/HttpApi.h \
    services/HttpApi/MySocketClient.h \
    services/HttpApi/SimpleClient.h \
    services/HttpApi/SimpleServer.h \
    services/SqlTable/SqlRecordCountTableModel.h \
    services/filewatcher/compactfilesystemwatcher.h \
    services/filewatcher/copyfilethread.h \
    services/filewatcher/RemoveFileThread.h \
    services/DatabaseManager/databasemanager.h \
    modules/dataLoader/catenarypicdataloader.h \
    modules/dataLoader/reviewjsondataloader.h \
    modules/maincontroller.h

DISTFILES += \
    UI/html/echarts.js \
    UI/html/countWebView_AI.html \
    UI/html/countWebView_Review.html \
    UI/html/trendWebView_AI.html \
    UI/html/trendWebView_Review.html

#===========================
#====start detect module====
#===========================
DEFINES -= UNICODE

#DEFINES += Linux

SOURCES += modules/detect/DetectServerThread.cpp

HEADERS += modules/detect/DetectServerThread.h

win32{
DEFINES += WIN32 \
           OPENCV \
           GPU

INCLUDEPATH = modules/detect/darknet \
              modules/detect/opencv/include

LIBS += -L$$PWD/modules/detect/lib/ \
        -lopencv_highgui412 \
        -lopencv_imgcodecs412 \
        -lopencv_imgproc412 \
        -lopencv_core412

LIBS += -L$$PWD/modules/detect/darknet/ \
        -lopencv_world3410 \
        -lyolo_cpp_dll
}

unix{
DEFINES += GPU CUDNN Linux

HEADERS += modules/detect/include/darknet.h

LIBS += /usr/local/lib/libopencv_highgui.so \
        /usr/local/lib/libopencv_highgui.so \
        /usr/local/lib/libopencv_imgcodecs.so \
        /usr/local/lib/libopencv_imgproc.so \
        /usr/local/lib/libopencv_core.so \
        /usr/local/lib/libopencv_videoio.so

LIBS += $$PWD/modules/detect/include/libdarknet.so \
        /usr/local/cuda/lib64/libcudart.so.10.2 \
        /usr/local/cuda/lib64/libcudnn.so.7 \
        /usr/local/cuda/lib64/libcurand.so.10 #\
        #/usr/local/cuda/lib64/libcublas.so.10.2

INCLUDEPATH += /usr/local/cuda/targets/x86_64-linux/include
}

DISTFILES += \
    modules/detect/Python/datachange.py \
    modules/detect/Python/Sample_Enhancement.py
#=========================
#====end detect module====
#=========================

TRANSLATIONS += railway-catenary-detect.ts
