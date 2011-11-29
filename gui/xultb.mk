
include platform.mk

XULTB_INCLUDES+=-Iinc
XULTB_INCLUDES+=-I../core/inc
DIRS=src/ui src/ui/core src/ui/core/list


XULTB_SOURCES+=$(wildcard $(addsuffix /*.c,$(DIRS)))
#XULTB_SOURCES+=$(wildcard test/*.c)

XULTB_CFLAGS+=-Wall

#XULTB_BASE=$(basename $(notdir $(XULTB_SOURCES)))
#XULTB_OBJECTS=$(addprefix obj/, $(addsuffix .o,$(XULTB_BASE)))
XULTB_OBJECTS=$(addsuffix .o, $(basename $(XULTB_SOURCES)))
XULTB_TARGET=libxultbgui.o
XULTB_TARGETA=libxultbgui.a
XULTB_TARGETSO=libxultbgui.so
XULTB_TEST_TARGETA=libxultbtest.a
XULTB_TARGET_XULTB_CFLAGS=

#all: obj_dir $(XULTB_TARGETA)  $(XULTB_TARGETSO)
all: obj_dir $(XULTB_TARGETA)
	#export LD_LIBRARY_PATH=$(shell pwd)

obj_dir:
	mkdir -p obj

ctags:
	ctags $(XULTB_SOURCES)
	
.c.o:
	$(CC) $(XULTB_CFLAGS) -c $(XULTB_INCLUDES) $< -o $@

$(XULTB_TARGETA): $(XULTB_OBJECTS)
	ar crv $@ $(XULTB_OBJECTS)
	nm $@ | wc

$(XULTB_TARGETSO): $(XULTB_OBJECTS)
	$(CC) -shared -o $@ $(XULTB_OBJECTS)

$(XULTB_TARGET): $(XULTB_SOURCES)
	$(CC) $(XULTB_CFLAGS) -c $(XULTB_INCLUDES) --combine $(XULTB_SOURCES) -o $(XULTB_TARGET)

test: $(XULTB_OBJECTS) src/test/test.o
	ar crv $(XULTB_TEST_TARGETA) $(XULTB_OBJECTS) src/test/test.o
#$(CC) $(XULTB_CFLAGS) -c $(XULTB_INCLUDES) --combine $(XULTB_SOURCES) src/test/test.c -o $(XULTB_TEST_TARGET)

install:

clean:
	rm -f $(XULTB_OBJECTS) $(XULTB_TARGETA) $(XULTB_TARGETSO)

.PHONY: test clean ctags obj_dir install

