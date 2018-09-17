# Requires SQLite 3 to be installed:
# `sudo apt-get install sqlite3 libsqlite3-dev`
# Optionally SQLite headers may be downloaded instead, and compiled as a dependency
# These will be excluded as it is assumed we are working on a system that has 
# the dependency preinstalled

CC=g++
CFLAGS=-lsqlite3 -std=c++11 -Wall -lpthread
OBJ=lumberjack.o
EXE=lumberjack
DB=octo
LOG=*.csv

all: $(OBJ)
	 $(CC) $(OBJ) $(CFLAGS) -o $(EXE)

lumberjack.o: lumberjack.cpp
	$(CC) -c lumberjack.cpp $(CFLAGS)

clean:
	rm -rf $(OBJ) $(EXE)
	
clean_all: clean
	rm -rf $(DB) $(LOG)

remake:
	make clean_all
	make all
