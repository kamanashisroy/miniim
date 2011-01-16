

all:
	make -C core
	make -C gui
	

test:
	make -C core test
	make -C gui test

clean:
	make -C core clean
	make -C gui clean

.PHONY: clean test
