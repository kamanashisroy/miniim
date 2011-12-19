
include ../../.config.mk
CURRENTPATH=$(XULTUBE_HOME)/platform/qt
include platform.mk

CFLAGS+=-Wall -ggdb3 -D_GNU_SOURCE

XULMAKE=$(MAKE) -f xultb.mk -C ../../core -I ../platform/qt
XULMAKE+=--quiet

XULGMAKE=$(MAKE) -f xultb.mk -C ../../gui -I ../platform/qt
XULGMAKE+=--quiet

XULMARKMAKE=$(MAKE) -f xultb.mk -C ../../web -I ../platform/qt
XULMARKMAKE+=--quiet

QTMAKETEMPLATE = "DEFINES += QT\r\n\
DEFINES += _GNU_SOURCE\r\n\
INCLUDEPATH += $(XULTUBE_HOME)/core/inc\r\n\
INCLUDEPATH += $(XULTUBE_HOME)/gui/inc\r\n\
INCLUDEPATH += $(CURRENTPATH)/inc\r\n\
LIBS += $(XULTUBE_HOME)/core/libxultbcore.a\r\n\
LIBS += $(XULTUBE_HOME)/gui/libxultbgui.a\r\n"



all:xultb qtprojectmake

xultb:
	$(XULMAKE) all
	$(XULGMAKE) all
	$(XULGMAKE) test

#$(XULMARKMAKE) all
#$(XULMARKMAKE) test

qtprojectmake:
	mkdir -p build
	$(ECHO) $(QTMAKETEMPLATE) > qtproject/.config.pro
	cd build;$(QMAKE) ../qtproject/qtproject.pro;make clean;make;make all;make staticlib

clean:
	$(XULMAKE) clean
	$(XULGMAKE) clean
	$(XULMARKMAKE) clean
	rm -f build/*
	

