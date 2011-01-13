
DIRS=src/test src/ui src/ui/core src/ui/core/list

INCLUDES=-Iinc
INCLUDES+=-I../core/inc
INCLUDES+=-I../obj_factory/inc
LIBS=-L../core/lib

SOURCES=$(wildcard $(addsuffix /*.c,$(DIRS)))
OBJECTS=$(addsuffix .o, $(basename $(SOURCES)))
TARGET=lib/xultb_gui.a
#INC=$(wildcard *.h)

all:$(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

.c.o:
	$(CC) -c $(INCLUDES) $< -o $@

test:
	$(CC) $(INCLUDES) $(LIBS) $(SOURCES) -o test
	./test

