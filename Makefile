# Requires SQLite 3 to be installed:
# sudo apt-get install sqlite3 libsqlite3-dev
# Optionally SQLite headers may be downloaded instead, and compiled as a dependency
# These will be excluded as it is assumed we are working on a debian based system

CC=g++
CFLAGS=-lsqlite3 -std=c++11 -Wall
OBJ=sqlite_driver.o
EXE=sqlite_driver

all: sqlite_driver.o
	 $(CC) $(OBJ) $(CFLAGS) -o $(EXE)

sqlite_driver.o: sqlite_driver.cpp
	$(CC) -c sqlite_driver.cpp $(CFLAGS)

clean:
	rm -rf $(OBJ) $(EXE)
	
remake:
	make clean
	make all
