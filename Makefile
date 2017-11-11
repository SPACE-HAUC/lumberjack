CC=g++
CFLAGS=-lsqlite -std=c++11 -Wall -Werror:

all:
	 test.cpp -lsqlite3
