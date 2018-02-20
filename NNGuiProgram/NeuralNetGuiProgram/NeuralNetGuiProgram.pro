#-------------------------------------------------
#
# Project created by QtCreator 2017-10-26T15:03:31
#
#-------------------------------------------------

QT       += core gui
QT       += sql widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = NeuralNetGuiProgram
TEMPLATE = app

CONFIG += extserialport


SOURCES += main.cpp\
        nnconfigwindow.cpp \
    customnn.cpp \
    neuron.cpp \
    loadtrainingdata.cpp \
    qcustomplot.cpp \
    executeneuralnet.cpp \
    mysqlapi.cpp \
    mysqlcustomquerywindow.cpp \
    mysqlsetupwindow.cpp \
    startupwindow.cpp \
    onlinenn.cpp \
    modbussetupwindow.cpp \
    runtime_window.cpp \
    connectionwidget.cpp \
    datalogdialogwindow.cpp \
    executelogicsetupwindow.cpp \
    controlobjectitemdelegate.cpp

HEADERS  += nnconfigwindow.h \
    customnn.h \
    neuron.h \
    loadtrainingdata.h \
    qcustomplot.h \
    executeneuralnet.h \
    mysqlapi.h \
    mysqlcustomquerywindow.h \
    mysqlsetupwindow.h \
    startupwindow.h \
    onlinenn.h \
    imodbus.h \
    modbussetupwindow.h \
    runtime_window.h \
    connectionwidget.h \
    datalogdialogwindow.h \
    executelogicsetupwindow.h \
    controlobjectitemdelegate.h

FORMS    += nnconfigwindow.ui \
    mysqlcustomquerywindow.ui \
    mysqlsetupwindow.ui \
    startupwindow.ui \
    modbussetupwindow.ui \
    runtime_window.ui \
    datalogdialogwindow.ui \
    executelogicsetupwindow.ui

#INCLUDEPATH += /home/reggie/Project_Apps/NeuralNet_Still/ModbusCustom/qmodbus/3rdparty/libmodbus \
#               /home/reggie/Project_Apps/NeuralNet_Still/ModbusCustom/qmodbus/3rdparty/libmodbus/src \
#            /home/reggie/Project_Apps/NeuralNet_Still/ModbusLinuxServer/libmodbus/src/

INCLUDEPATH += "/usr/local/include/"

INCLUDEPATH += "/home/reggie/Project_Apps/gitDownloads/nlohmann3.0.0/json"

LIBS += -L/usr/local/lib -lmodbus

#unix {
#    SOURCES += 3rdparty/qextserialport/posix_qextserialport.cpp	\
#           3rdparty/qextserialport/qextserialenumerator_unix.cpp
#    DEFINES += _TTY_POSIX_
#}

LIBS += -L/usr/lib/x86_64-linux-gnu -lmysqlclient -lpthread -lz -lm -lrt -latomic -ldl \
-luuid \
-lboost_system -lboost_filesystem

RESOURCES +=

