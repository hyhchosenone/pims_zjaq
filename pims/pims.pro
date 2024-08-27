QT       += core gui sql network texttospeech printsupport serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
TARGET = pims
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    alarmdata.cpp \
    alarmpixmapitem.cpp \
    alarmreciver.cpp \
    alarmscence.cpp \
    alarmtcpserver.cpp \
    alarmthread.cpp \
    alarmwindows.cpp \
    alarmworkerthread.cpp \
    apiworker.cpp \
    datawindows.cpp \
    dbhelper.cpp \
    defence.cpp \
    defencedialog.cpp \
    frmabout.cpp \
    frmconfig.cpp \
    gohelper.cpp \
    hvdata.cpp \
    iconfonthelper.cpp \
    main.cpp \
    netalarmserver.cpp \
    netclientworker.cpp \
    pims.cpp \
#    rtsplayer.cpp \
    tcpclient.cpp \
    vibsensor.cpp \
    vibsensorworker.cpp \
    xsqlquerymodel.cpp \
    xtablewidgetitem.cpp

HEADERS += \
    alarmdata.h \
    alarminfo.h \
    alarmpixmapitem.h \
    alarmreciver.h \
    alarmscence.h \
    alarmtcpserver.h \
    alarmthread.h \
    alarmwindows.h \
    alarmworkerthread.h \
    apiworker.h \
    datawindows.h \
    dbhelper.h \
    defence.h \
    defencedialog.h \
    # dw.h \
    frmabout.h \
    frmconfig.h \
    gohelper.h \
    hvdata.h \
    iconfonthelper.h \
    netalarmserver.h \
    netclientworker.h \
    pims.h \
    protocol.h \
    protocol_v3.h \
    resource.h \
#    rtsplayer.h \
    tcpclient.h \
    util.h \
    vibsensor.h \
    vibsensorworker.h \
    xsqlquerymodel.h \
    xtablewidgetitem.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    pims.qrc

FORMS += \
    alarmwindows.ui \
    datawindows.ui \
    defencedialog.ui \
    frmabout.ui \
    frmconfig.ui \
    pims.ui \
    rtsplayer.ui

DISTFILES += \
    PIMS.ico \
    classuml.qmodel

unix:!macx: LIBS += -L$$OUT_PWD/../GoPDF/ -lGoPDF

INCLUDEPATH += $$PWD/../GoPDF
DEPENDPATH += $$PWD/../GoPDF

unix:!macx: LIBS += -L$$OUT_PWD/../GoUI/ -lGoUI

INCLUDEPATH += $$PWD/../GoUI
DEPENDPATH += $$PWD/../GoUI
