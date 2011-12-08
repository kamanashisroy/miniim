
include ../../.config.mk
include ../../platform/qt/platform.mk
include libs.mk

CC=g++
INCLUDES+=$(XULTB_INCLUDES)

TARGET=qt_test.bin

STATIC_LIBS+=../../core/libxultbcore.a
STATIC_LIBS+=../../gui/libxultbgui.a
STATIC_LIBS+=../../gui/libxultbtest.a
STATIC_LIBS+=../../platform/qt/build/libqtguiproject.a
LIBS+=$(STATIC_LIBS)


PQTMAKE=$(MAKE) -C ../../platform/qt
PQTMAKE+=--quiet

all: qtsharedlib $(TARGET)
	echo -e "export LD_LIBRARY_PATH=$(QT_HOME)/lib"
	@echo "You may want to execute the line above in the shell and then try $(TARGET)"

obj_dir:
	mkdir -p obj

ctags:
	ctags $(SOURCES)
	
$(TARGET): $(OBJECTS) build_extobjs
	$(CC) $(CFLAGS) -o $(TARGET) $(wildcard extobjs/*.o) $(LDFLAGS) $(LIBS)

qtsharedlib:
	$(PQTMAKE) all

build_extobjs:
	mkdir -p extobjs
	cp $(STATIC_LIBS) extobjs/
	$(MAKE) -C extobjs

clean:
	$(PQTMAKE) clean
	rm -f $(TARGET) $(OBJECTS)
	$(MAKE) -C extobjs clean

.PHONY: test clean ctags obj_dir all
