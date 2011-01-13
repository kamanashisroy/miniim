

INCLUDES=-Iinc

SOURCES+=$(wildcard src/*.c)
SOURCES+=$(wildcard test/*.c)

TEST_BIN=test_obj_factory_utils

CFLAGS=-Wall -ggdb3

TEST_CFLAGS=-DTEST_OBJ_FACTORY_UTILS

all:

ctags:
	ctags $(SOURCES) $(INCLUDES)
	

test:
	$(CC) $(CFLAGS) $(TEST_CFLAGS) $(INCLUDES) $(SOURCES) -o $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm $(TEST_BIN)

.PHONY: test clean ctags
