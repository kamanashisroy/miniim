
ARCHIVES=$(wildcard *.a)
BASE=$(basename $(notdir $(ARCHIVES)))
TARGETS=$(addprefix .,$(addsuffix .nothing,$(BASE)))

all:$(TARGETS)

.%.nothing:%.a
	ar -x $^
	touch $@

clean:
	rm -f *.a *.o *.nothing
	
