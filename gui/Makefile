
DIRS=src/test src/ui src/ui/core src/ui/core/list

INCLUDES=-Iinc
INCLUDES+=-I../core/inc
INCLUDES+=-I../obj_factory/inc
LIBS=-L../core/lib -lxultb_core.a

SOURCES=$(wildcard $(addsuffix /*.c,$(DIRS)))
OBJECTS=$(addsuffix .o, $(basename $(SOURCES)))
TARGET=lib/xultb_gui.a
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
	$(CC) $(INCLUDES) $(LIBS) $(SOURCES) -o test
	./test

clean:
	rm -f $(TEST_BIN)
	rm -f $(OBJECTS)
	rm -f $(TARGET)

.PHONY: test clean ctags
