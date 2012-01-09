
include ../../.config.mk
CURRENTPATH=$(XULTUBE_HOME)/platform/qt
include platform.mk

CFLAGS+=-Wall -ggdb3 -D_GNU_SOURCE

XULMAKE=$(MAKE) -f xultb.mk -C ../../core -I ../platform/qt
XULMAKE+=--quiet

XULGMAKE=$(MAKE) -f xultb.mk -C ../../gui -I ../platform/qt
XULGMAKE+=--quiet

XULPARSERMAKE=$(MAKE) -f xultb.mk -C ../../ml -I ../platform/qt
XULPARSERMAKE+=--quiet

XULPAGEMAKE=$(MAKE) -f xultb.mk -C ../../web -I ../platform/qt
XULPAGEMAKE+=--quiet

QTMAKETEMPLATE = "DEFINES += QT\r\n\
DEFINES += _GNU_SOURCE\r\n\
INCLUDEPATH += $(XULTUBE_HOME)/core/inc\r\n\
INCLUDEPATH += $(XULTUBE_HOME)/gui/inc\r\n\
INCLUDEPATH += $(CURRENTPATH)/inc\r\n\
LIBS += $(CURRENTPATH)/extobjs/resolder.a\r\n"


STATIC_LIBS=../../core/libxultbcore.a
STATIC_LIBS+=../../gui/libxultbgui.a


all:xultb qtprojectmake

xultb:
	$(XULMAKE) all
	$(XULGMAKE) all
	$(XULGMAKE) test
	$(XULPAGEMAKE) all
	$(XULPAGEMAKE) test
	$(XULPARSERMAKE) all

build:
	mkdir -p build

qtprojectmake:build resolder
	$(ECHO) $(QTMAKETEMPLATE) > qtproject/.config.pro
	cp Makefile.build build
	$(MAKE) -C build -f Makefile.build -I $(XULTUBE_HOME)/platform/qt

#cd build;$(QMAKE) ../qtproject/qtproject.pro;make clean;make;make all;make staticlib

clean:
	$(XULMAKE) clean
	$(XULGMAKE) clean
	$(XULPAGEMAKE) clean
	$(XULPARSERMAKE) clean
	$(REMOVE_FILE) -r build/*
	$(MAKE) -C extobjs clean
	
resolder:
	cp $(STATIC_LIBS) extobjs/
	$(MAKE) -C extobjs

