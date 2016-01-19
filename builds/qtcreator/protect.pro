# Copyright (c) 2012-2013 by RecMind Team. All rights reserved.

TEMPLATE = lib
TARGET = protect
QT += core gui xml svg network
CONFIG += staticlib
DEFINES += QT_LARGEFILE_SUPPORT QT_XML_LIB QT_SVG_LIB QT_NETWORK_LIB

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
               $$PRJROOT/src/protect \
               $$MOC_DIR \
               $$UI_DIR

LIBS += -lz
DEPENDPATH += .

include( $$PRJROOT/builds/qtcreator/protect.pri)
