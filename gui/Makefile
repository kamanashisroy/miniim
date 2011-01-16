
DIRS=src/ui src/ui/core src/ui/core/list

INCLUDES=-Iinc
INCLUDES+=-I../core/inc
INCLUDES+=-I../obj_factory/inc
LIBS=-L../core/lib -lxultb_core

SOURCES=$(wildcard $(addsuffix /*.c,$(DIRS)))
OBJECTS=$(addsuffix .o, $(basename $(SOURCES)))
TARGET=lib/libxultb_gui.a
CFLAGS=-Wall -ggdb3

TEST_CFLAGS=-DTEST_XULTB_GUI
TEST_BIN=test.bin

all:$(OBJECTS)
	ar rcs $(TARGET) $(OBJECTS)

.c.o:
	$(CC) $(CFLAGS) -c $(INCLUDES) $< -o $@

ctags:
	ctags $(SOURCES) $(INCLUDES)

test:
	$(CC) $(INCLUDES) $(LIBS) $(SOURCES) src/test/test.c -o test
	./test

clean:
	rm -f $(TEST_BIN)
	rm -f $(OBJECTS)
	rm -f $(TARGET)

.PHONY: test clean ctags
