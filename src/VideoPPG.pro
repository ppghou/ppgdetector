#-------------------------------------------------
#
# Project created by QtCreator 2018-12-19T00:36:24
#
#-------------------------------------------------

QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VideoPPG
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += /home/my/software/sigpack-1.2.4/sigpack
LIBS += -L/usr/local/lib -ldlib -lcblas -llapack -lpthread \
     -lX11 -lopencv_core -lopencv_video -lopencv_videostab \
    -lopencv_calib3d -lopencv_dnn -lopencv_features2d \
    -lopencv_features2d -lopencv_flann -lopencv_highgui \
    -lopencv_imgcodecs -lopencv_imgproc -lopencv_ml \
    -lopencv_objdetect -lopencv_photo -lopencv_shape \
    -lopencv_stitching -lopencv_videoio -lvlc

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    /home/my/software/dlib-master/dlib/all/source.cpp \
    ppgFilter.cpp \
    ppgDetector.cpp \
    ppgController.cpp

HEADERS += \
    mainwindow.h \
    utils.h \
    ppgFilter.h \
    ppgDetector.h \
    ppgController.h

FORMS += \
        mainwindow.ui
