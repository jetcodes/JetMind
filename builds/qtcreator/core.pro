# Copyright (c) 2012-2013 by RecMind Team. All rights reserved.

TEMPLATE = lib
TARGET = core
QT += core gui xml script xmlpatterns svg
DEFINES += QT_XML_LIB QT_SVG_LIB PRODRAWCORE_LIB

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
               $$PRJROOT/src/core \
               $$MOC_DIR \
               $$UI_DIR

DEPENDPATH += .

include( $$PRJROOT/builds/qtcreator/core.pri)

win32 {
    LIBS += -L$$PRJROOT/builds/build-jetmind/$$MODE/ -lquazip
}

macx {
    CONFIG += staticlib
}
