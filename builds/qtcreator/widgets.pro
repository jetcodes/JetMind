# Copyright (c) 2012-2013 by RecMind Team. All rights reserved.

TEMPLATE = lib
TARGET = widgets
QT += core gui xml svg
DEFINES += QT_LARGEFILE_SUPPORT QT_XML_LIB QT_SVG_LIB

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
               $$PRJROOT/src/widgets \
               $$MOC_DIR \
               $$UI_DIR \
               $$PRJROOT/builds/build-jetmind/$$MODE/.ui

DEPENDPATH += .

include( $$PRJROOT/builds/qtcreator/widgets.pri)

# under windows, we use dynamic link library
win32 {
    LIBS += -L$$PRJROOT/builds/build-jetmind/$$MODE/ -lcore
    DEFINES += WIDGETS_LIB
}

# only in unix/linux system, use static link
macx {
    CONFIG += staticlib
}
