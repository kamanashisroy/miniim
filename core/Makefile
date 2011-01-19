
DIRS+=src/core

INCLUDES=-Iinc
SOURCES=$(wildcard $(addsuffix /*.c,$(DIRS)))
OBJECTS=$(addsuffix .o, $(basename $(SOURCES)))
TARGET=lib/libxultb_core.a
TARGET_SO=lib/libxultb_core.so
CFLAGS=-Wall -ggdb3

TEST_CFLAGS=-DTEST_OBJ_FACTORY_UTILS -lpthread
TEST_BIN=test.bin

all:$(OBJECTS)
	ar crv $(TARGET) $(OBJECTS)
	$(CC) --shared $(OBJECTS) -o $(TARGET_SO)

.c.o:
	$(CC) $(CFLAGS) -c $(INCLUDES) $< -o $@

ctags:
	ctags $(SOURCES) $(INCLUDES)

test:
	$(CC) $(CFLAGS) $(TEST_CFLAGS) $(INCLUDES) $(SOURCES) src/test/test.c -o $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -f $(TEST_BIN)
	rm -f $(OBJECTS)
	rm -f $(TARGET)
	rm -f $(TARGET_SO)

.PHONY: test clean ctags
