#-------------------------------------------------
#
# Project created by QtCreator 2017-09-17T21:07:55
#
#-------------------------------------------------

QT       += core gui    network
QT      += testlib

DEFINES += IMX_GPIO

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtApp
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


INCLUDEPATH += ../ \
               ./


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    globalcommon.cpp \
    poweronselftest.cpp \
    common/qled.cpp \
    common/channelindicatorbutton.cpp \
    voltageset.cpp \
    channeltest.cpp \
    channelseclet.cpp \
    bridgecircuit.cpp \
    insulation.cpp \
    releasepage.cpp \
    cutoffpage.cpp \
    common/cchildqwidget.cpp \
    common/basetable.cpp \
    common/rthreadobj.cpp \
    worker/backworkthread.cpp \
    device/docontroller.cpp \
    qmodbus/qmodbusbits.cpp \
    qmodbus/qmodbuserror.cpp \
    qmodbus/qmodbusmaster.cpp \
    qmodbus/qmodbusregisters.cpp \
    qmodbus/libmodbus/modbus.c \
    qmodbus/libmodbus/modbus-data.c \
    qmodbus/libmodbus/modbus-rtu.c \
    qmodbus/libmodbus/modbus-tcp.c \
    device/restester.cpp \
    common/showmessageboxfromotherthread.cpp \
    common/confirmw.cpp \
    debuglogform.cpp \
    device/gpio.cpp \
    worker/task.cpp \
    common/saveresults.cpp \
    newthread.cpp

HEADERS += \
        mainwindow.h \
    globalcommon.h \
    poweronselftest.h \
    common/qled.h \
    common/channelindicatorbutton.h \
    voltageset.h \
    channeltest.h \
    channelseclet.h \
    bridgecircuit.h \
    insulation.h \
    releasepage.h \
    cutoffpage.h \
    common/cchildqwidget.h \
    common/basetable.h \
    common/rthreadobj.h \
    worker/backworkthread.h \
    device/docontroller.h \
    qmodbus/qmodbus.h \
    qmodbus/qmodbusbits.h \
    qmodbus/qmodbuserror.h \
    qmodbus/qmodbusmaster.h \
    qmodbus/qmodbusregisters.h \
    qmodbus/libmodbus/config.h \
    qmodbus/libmodbus/modbus.h \
    qmodbus/libmodbus/modbus-private.h \
    qmodbus/libmodbus/modbus-rtu.h \
    qmodbus/libmodbus/modbus-rtu-private.h \
    qmodbus/libmodbus/modbus-tcp.h \
    qmodbus/libmodbus/modbus-tcp-private.h \
    qmodbus/libmodbus/modbus-version.h \
    device/restester.h \
    common/showmessageboxfromotherthread.h \
    common/confirmw.h \
    debuglogform.h \
    device/gpio.h \
    worker/task.h \
    common/saveresults.h \
    newthread.h

FORMS += \
        mainwindow.ui \
    poweronselftest.ui \
    voltageset.ui \
    channeltest.ui \
    channelseclet.ui \
    cutoffpage.ui \
    common/basetable.ui \
    common/confirmw.ui \
    debuglogform.ui

RESOURCES += \
    res.qrc \
    channelindicatorbuttonrc.qrc \
    qss.qrc

INCLUDEPATH += ./common \
            ./device \
            ./worker \
            ./qmodbus \
            ./qmodbus/libmodbus

OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build

DESTDIR = ../QAppBin

greaterThan(QT_MAJOR_VERSION, 4):DESTDIR = ../QAppBinV5

#QModbus
#{
#LIBS += -L../QAppBin
#CONFIG(debug, debug|release) {
#                LIBS += -lQModbusd
#}
#else {
#                LIBS += -lQModbus

#        QMAKE_CXXFLAGS += -fno-strict-aliasing
#        #LFLAGS += -static-libgcc
#}
#INCLUDEPATH += ../QModbus

#}
win32: LIBS += -Ldll -lws2_32  #windows only
