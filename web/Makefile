
DIRS=src/test src/ui/markup src/ui/markup/extended

INCLUDES=-Iinc -I../gui/inc
SOURCES=$(wildcard $(addsuffix /*.c,$(DIRS)))
#INC=$(wildcard *.h)

all:

test:
	gcc $(INCLUDES) $(SOURCES) -o test
	./test

