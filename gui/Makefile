
DIRS=src/ui src/ui/core src/ui/core/list

INCLUDES=-Iinc
INCLUDES+=-I../core/inc
INCLUDES+=-I../obj_factory/inc
#LIBS=-L../core/lib -lxultb_core


SOURCES=$(wildcard $(addsuffix /*.c,$(DIRS)))
OBJECTS=$(addsuffix .o, $(basename $(SOURCES)))
EXT_OBJECTS+=../core/lib/libxultb_core.o
TARGET=lib/libxultb_gui.a
TARGET_A=lib/libxultb_gui.a
TARGET_SO=lib/libxultb_gui.so
CFLAGS=-Wall -ggdb3

TEST_CFLAGS=-DTEST_XULTB_GUI
TEST_BIN=test.bin

all:$(OBJECTS)
	ar crv $(TARGET_A) $(OBJECTS)
	$(CC) --shared $(OBJECTS) -o $(TARGET_SO)
	$(CC) $(CFLAGS) -c $(INCLUDES) --combine $(SOURCES) -o $(TARGET)
	nm $(TARGET) | wc

.c.o:
	$(CC) $(CFLAGS) -c $(INCLUDES) $< -o $@

ctags:
	ctags $(SOURCES) $(INCLUDES)

test:
	$(CC) $(CFLAGS) $(TEST_CFLAGS) -c $(INCLUDES) --combine $(SOURCES) src/test/test.c -o $(TEST_BIN).o
	nm $(TEST_BIN).o | wc
	$(CC) -o $(TEST_BIN) $(EXT_OBJECTS) $(TEST_BIN).o
	./$(TEST_BIN)

clean:
	rm -f $(TEST_BIN)
	rm -f $(OBJECTS)
	rm -f $(TARGET)
	rm -f $(TARGET_A)
	rm -f $(TARGET_SO)

.PHONY: test clean ctags
