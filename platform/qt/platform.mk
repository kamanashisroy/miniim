

include ../../.config.mk
CC=g++
QMAKE=$(QT_HOME)/bin/qmake
#BITFLAG=-DSYNC_BIT64
#OPTFLAG=-O2 -Wall
OPTFLAG=-ggdb3 -Wall
CFLAGS=-D_GNU_SOURCE -DQTGUI_LIBRARY
CFLAGS+=$(BITFLAG)
CFLAGS+=$(OPTFLAG)
INCLUDES=-Iinc -I$(QT_HOME)/include
XULTB_CFLAGS=-D_GNU_SOURCE -DQTGUI_LIBRARY
XULTB_INCLUDES+=-I$(QT_HOME)/include -I$(XULTUBE_HOME)/platform/qt/qtproject/inc
XULTB_CFLAGS+=$(BITFLAG)
XULTB_CFLAGS+=$(OPTFLAG)
#CC=g++


