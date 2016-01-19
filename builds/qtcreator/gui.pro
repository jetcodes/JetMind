# Copyright (c) 2012-2013 by RecMind Team. All rights reserved.

TEMPLATE = app
TARGET = JetMind
QT += core gui xml svg network
DEFINES += QT_LARGEFILE_SUPPORT QT_SVG_LIB QT_DLL QT_XML_LIB QT_NETWORK_LIB

PRJROOT = ./../..

DESTDIR = $$OUT_PWD/Debug
MODE = Debug

# if it is release mode then we should change it
CONFIG(release, debug|release) {
    DESTDIR = $$OUT_PWD/Release
    MODE = Release
}

MOC_DIR     += $$DESTDIR/.moc
OBJECTS_DIR += $$DESTDIR/.obj
UI_DIR      += $$DESTDIR/.ui
RCC_DIR     += $$DESTDIR/.qrc

INCLUDEPATH += $$PRJROOT/include \
               $$PRJROOT/src/gui \
               $$MOC_DIR \
               $$UI_DIR \
               $$PRJROOT/builds/build-jetmind/$$MODE/.ui

DEPENDPATH += .

include($$PRJROOT/builds/qtcreator/gui.pri)

LIBS += -L$$PRJROOT/builds/build-jetmind/$$MODE/ -lcore \
        -L$$PRJROOT/builds/build-jetmind/$$MODE/ -lquazip \
        -L$$PRJROOT/builds/build-jetmind/$$MODE/ -lwidgets \
        -L$$PRJROOT/builds/build-jetmind/$$MODE/ -lmindstorm

win32 {
    RESOURCES += ../../res/win/res.qrc
    HEADERS += ../../src/gui/CTitleBar.h \
               ../../src/gui/CUiWidget_win.h
    SOURCES += ../../src/gui/CUiWidget_win.cpp \
               ../../src/gui/CTitleBar.cpp
    FORMS += ../../src/gui/CTitleBar.ui
}

macx {
    RESOURCES += ../../res/mac/res.qrc
    HEADERS += ../../src/gui/CUiWidget_mac.h
    SOURCES += ../../src/gui/CUiWidget_mac.cpp
    LIBS += -lz
}
