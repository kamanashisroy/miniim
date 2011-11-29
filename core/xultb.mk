
include platform.mk

DIRS+=src/core
XULTB_INCLUDES+=-Iinc

XULTB_SOURCES+=$(wildcard $(addsuffix /*.c,$(DIRS)))
#XULTB_SOURCES+=$(wildcard test/*.c)

XULTB_CFLAGS+=-Wall

#XULTB_BASE=$(basename $(notdir $(XULTB_SOURCES)))
#XULTB_OBJECTS=$(addprefix obj/, $(addsuffix .o,$(XULTB_BASE)))
XULTB_OBJECTS=$(addsuffix .o, $(basename $(XULTB_SOURCES)))
XULTB_TARGET=libxultbcore.o
XULTB_TARGETA=libxultbcore.a
XULTB_TARGETSO=libxultbcore.so
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

test:
	$(CC) $(XULTB_CFLAGS) $(TEST_CFLAGS) $(XULTB_INCLUDES) $(XULTB_SOURCES) src/test/test.c -o $(TEST_BIN)
	./$(TEST_BIN)

install:

clean:
	rm -f $(XULTB_OBJECTS) $(XULTB_TARGETA) $(XULTB_TARGETSO)

.PHONY: test clean ctags obj_dir install

