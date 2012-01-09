
ARCHIVES=$(wildcard *.a)
BASE=$(basename $(notdir $(ARCHIVES)))
TARGETS=$(addprefix .,$(addsuffix .nothing,$(BASE)))

TARGET=resolder.a

all:$(TARGETS) $(TARGET)

.%.nothing:%.a
	ar x $^
	touch $@

$(TARGET):
	ar cr $(TARGET) *.o

clean:
	rm -f *.a *.o *.nothing
	
