
DIRS=src/test

INCLUDES=-Iinc
SOURCES=$(wildcard $(addsuffix /*.c,$(DIRS)))
#INC=$(wildcard *.h)

all:

test:
	gcc $(INCLUDES) $(SOURCES) -o test
	./test

clean:
	rm -f test

