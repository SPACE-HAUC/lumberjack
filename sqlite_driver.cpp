#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <sqlite3.h>

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
	for (int i = 0; i < argc; ++i) {
		printf("Column name: %s - Data: %s\n", azColName[i], argv[i]);
	}
	return 0;
}

int main(int argc, char*argv[]) {
	sqlite3 *db;
	char* zErrMsg = 0;
	// Open the SQLite database 
	int rc = sqlite3_open("octo", &db);
	printf("%d\n", rc);
	if (rc) {
		std::cout << "There is an issue with SQLite. Exiting.";
		exit(1);
	}

	rc = sqlite3_exec(db, "SELECT * FROM test", callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		std::cout << "Failed to write command\n";
		std::cout << zErrMsg;
		sqlite3_free(zErrMsg);
	}
	sqlite3_close(db);
	return 0;
}
