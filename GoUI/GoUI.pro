QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib
DEFINES += GOUI_LIBRARY

CONFIG += c++11

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
    app.cpp \
    appinit.cpp \
    applog.cpp \
    combobox.cpp \
    defencebutton.cpp \
    frmmessagebox.cpp \
    goui.cpp \
    iconhelper.cpp \
    imageview.cpp \
    qdelegate.cpp \
    stdafx.cpp \
    switchbutton.cpp

HEADERS += \
    GoUI_global.h \
    app.h \
    appinit.h \
    applog.h \
    combobox.h \
    defencebutton.h \
    frmmessagebox.h \
    goui.h \
    iconhelper.h \
    imageview.h \
    myhelper.h \
    qdelegate.h \
    stdafx.h \
    switchbutton.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

FORMS += \
    frmmessagebox.ui
