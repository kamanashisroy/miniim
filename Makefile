

all:
	$(MAKE) -C apps/qt_test

#make -C core
#make -C gui
	

test:
	make -C core test
	make -C gui test

clean:
	$(MAKE) -C apps/qt_test clean

#make -C core
#make -C core clean
#make -C gui clean

.PHONY: clean test
