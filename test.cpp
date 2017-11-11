#include <iostream>
#include <sqlite3.h>

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
	for (int i = 0; i < argc; ++i) {
		std::cout << azColName[i] << argv[i] ? argv[i] : NULL;
	}
}

int main(int argc, char*argv[]) {
	sqlite3 *db;
	char* zErrMsg = 0;
	int rc = sqlite3_open("octo", &db);
	if (rc) {
		std::cout << "Failed cry\n";
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
