
#QTLIBS+=-F$(QT_HOME)/lib -L$(QT_HOME)/lib -framework QtMultimedia -framework QtNetwork -framework QtCore 
QTLIBS+=-L$(QT_HOME)/lib -lQtMultimedia -lQtNetwork -lQtCore -lQtGui
LIBS+=$(QTLIBS)

# for shared linking
#ifdef LUA_HOME
#LUALIBS+=-llua # for shared linking
#INCLUDES+=-I$(LUA_HOME)/include
#else
#LUALIBS+=$(LUA_LIB)
#INCLUDES+=$(LUA_INCLUDE)
#endif

# add lua libs -L...
#LUALIBS+=-lm -lreadline
#LIBS+=$(LUALIBS)


