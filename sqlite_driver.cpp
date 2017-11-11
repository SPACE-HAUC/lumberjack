#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <sqlite3.h>

#include "sqlite_driver.hpp"

static int readCallback(void *NotUsed, int argc, char **argv, char **azColName) {
	for (int i = 0; i < argc; ++i) {
		printf("Column name: %s - Data: %s\n", azColName[i], argv[i]);
	}
	return 0;
}

static int writeCallback(void *NotUsed, int argc, char **argv, char **azColName) {
	for (int i = 0; i < argc; ++i) {
		printf("Column name: %s - Data: %s\n", azColName[i], argv[i]);
	}
	return -1;
}

int readFromDatabase() {
	char* zErrMsg = 0;
	int rc = sqlite3_exec(db, "SELECT * FROM test", readCallback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		std::cout << "Failed to write command\n";
		std::cout << zErrMsg;
		sqlite3_free(zErrMsg);
		return -1;
	}
	return 0;
}

int writeToDatabase() {
	char* zErrMsg = 0;
	int rc = sqlite3_exec(db, "INSERT INTO test (name, age) VALUES (\"test insert\", 75)", writeCallback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		std::cout << "Failure to write command\n";
		std::cout << zErrMsg;
		sqlite3_free(zErrMsg);
		return -1;
	}
	return 0;
}

void init() {
	// Open the SQLite database 
	int rc = sqlite3_open("octo", &db);
	if (rc != SQLITE_OK) {
		std::cout << "There is an issue with SQLite. Exiting.\n";
		exit(1);
	}
}

void closeDatabase() {
	sqlite3_close(db);
}

int main(int argc, char*argv[]) {
	init();
	writeToDatabase();
	//readFromDatabase();
	closeDatabase();
	return 0;
}



