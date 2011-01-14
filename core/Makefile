
DIRS+=src/core

INCLUDES=-Iinc
SOURCES=$(wildcard $(addsuffix /*.c,$(DIRS)))
OBJECTS=$(addsuffix .o, $(basename $(SOURCES)))
TARGET=lib/xultb_core.a

TEST_CFLAGS=-DTEST_OBJ_FACTORY_UTILS -lpthread
TEST_BIN=test.bin

all:$(OBJECTS)
	ar rcs $(TARGET) $(OBJECTS)

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

.PHONY: test clean ctags
