CC = g++
CFLAGS = $(shell mysql_config --cflags)
LIBS = $(shell mysql_config --libs)
OBJECTS = server

.PHONY : all clean

all : $(OBJECTS)

server : Definition.h Database.hpp Log.hpp DealWithMsg.hpp Server.cpp
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean :
	rm $(OBJECTS)
	rm -rf log