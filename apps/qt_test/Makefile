
include ../../.config.mk
include ../../platform/qt/platform.mk
include libs.mk

CC=g++
INCLUDES+=$(XULTB_INCLUDES)

TARGET=qt_test.bin

STATIC_LIBS+=../../core/libxultbcore.a
#STATIC_LIBS+=../../gui/libxultbgui.a
STATIC_LIBS+=../../gui/libxultbtest.a
ifeq ("$(PLATFORM)","win")
STATIC_LIBS+=../../platform/qt/build/debug/libqtguiproject.a
else
STATIC_LIBS+=../../platform/qt/build/libqtguiproject.a
endif
LIBS+=$(STATIC_LIBS)


PQTMAKE=$(MAKE) -C ../../platform/qt
PQTMAKE+=--quiet

all: qtsharedlib $(TARGET)
	$(ECHO) "In linux export the library path as:"
	$(ECHO) "export LD_LIBRARY_PATH=$(QT_HOME)/lib"
	$(ECHO) "In windows export the library path as:"
	$(ECHO) "set PATH=%PATH%;$(QT_HOME)/lib;$(XULTUBE_HOME)/platform/qt/build/debug/"
	@$(ECHO) "You may want to execute the line above in the shell and then try $(TARGET)"

ctags:
	ctags $(SOURCES)
	
$(TARGET): $(OBJECTS) resolder
	$(CC) $(CFLAGS) extobjs/resolder.a -o $(TARGET) $(LDFLAGS) $(LIBS)

qtsharedlib:
	$(PQTMAKE) all

resolder:
	cp $(STATIC_LIBS) extobjs/
	$(MAKE) -C extobjs

clean:
	$(PQTMAKE) clean
	rm -f $(TARGET) $(OBJECTS)
	$(MAKE) -C extobjs clean

.PHONY: test clean ctags obj_dir all
