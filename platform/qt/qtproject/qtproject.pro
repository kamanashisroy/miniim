#-------------------------------------------------
#
# Project created by QtCreator 2011-06-30T00:20:01
#
#-------------------------------------------------

QT       += gui

TARGET = qtguiproject
TEMPLATE = lib

include(.config.pro)

DEFINES += QTGUI_LIBRARY

SOURCES += src/ui/core/qt_graphics.cpp \
    src/ui/core/qt_platform.cpp \
    src/ui/core/qt_window.cpp

HEADERS += inc/qt_config.h inc/qt_graphics.h inc/qt_window.h inc/qt_logger.h

INCLUDEPATH+=inc


CXXFLAGS+=-ggdb3




