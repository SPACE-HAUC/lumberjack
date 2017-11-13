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
	int rc = sqlite3_exec(db, "INSERT INTO power VALUES (null, 4.5, 6.7)", writeCallback, 0, &zErrMsg);
	// NSERT INTO salespeople VALUES (null, 'Fred', 'Flinstone', 10.0);";
	
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
	int rc = sqlite3_open_v2("octo", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
	if (rc != SQLITE_OK) {
		std::cout << "There is an issue with SQLite. Exiting.\n";
		exit(1);
	}
	const char* acsTable = "CREATE TABLE IF NOT EXISTS acs ( \
										id INTEGER PRIMARY KEY AUTOINCREMENT,\
										mag_x FLOAT,\
										mag_y FLOAT,\
										mag_z FLOAT,\
										gyro_x FLOAT,\
										gyro_y FLOAT,\
										gyro_z FLOAT,\
										light_sensor_1 FLOAT,\
										light_sensor_2 FLOAT,\
										light_sensor_3 FLOAT,\
										light_sensor_4 FLOAT,\
										light_sensor_5 FLOAT,\
										light_sensor_6 FLOAT,\
										light_sensor_7 FLOAT,\
										light_sensor_8 FLOAT,\
										SUN_SENSOR_DARK BOOL);";
	const char* beamSteeringTable = "CREATE TABLE IF NOT EXISTS beam_steering ( \
											id INTEGER PRIMARY KEY AUTOINCREMENT,\
											phase_beam_1 FLOAT,\
											phase_beam_2 FLOAT,\
											phase_beam_3 FLOAT,\
											phase_beam_4 FLOAT,\
											phase_beam_5 FLOAT,\
											phase_beam_6 FLOAT,\
											phase_beam_7 FLOAT,\
											phase_beam_8 FLOAT,\
											phase_beam_9 FLOAT,\
											phase_beam_10 FLOAT,\
											phase_beam_11 FLOAT,\
											phase_beam_12 FLOAT,\
											phase_beam_13 FLOAT,\
											phase_beam_14 FLOAT,\
											phase_beam_15 FLOAT,\
											phase_beam_16 FLOAT);";
	const char* powerTable = "CREATE TABLE IF NOT EXISTS power ( \
								id INTEGER PRIMARY KEY AUTOINCREMENT,\
								net_power FLOAT,\
								future_power FLOAT);";
	sqlite3_exec(db, acsTable, nullptr, nullptr, nullptr);
	sqlite3_exec(db, beamSteeringTable, nullptr, nullptr, nullptr);
	sqlite3_exec(db, powerTable, nullptr, nullptr, nullptr);
}

void closeDatabase() {
	loadOrSaveDb(db, "octo", 1);
	sqlite3_close(db);
}


int loadOrSaveDb(sqlite3 *pInMemory, const char *zFilename, int isSave){
  int rc;                   /* Function return code */
  sqlite3 *pFile;           /* Database connection opened on zFilename */
  sqlite3_backup *pBackup;  /* Backup object used to copy data */
  sqlite3 *pTo;             /* Database to copy to (pFile or pInMemory) */
  sqlite3 *pFrom;           /* Database to copy from (pFile or pInMemory) */
  /* Open the database file identified by zFilename. Exit early if this fails
  ** for any reason. */
  rc = sqlite3_open(zFilename, &pFile);
  if( rc==SQLITE_OK ){

    /* If this is a 'load' operation (isSave==0), then data is copied
    ** from the database file just opened to database pInMemory. 
    ** Otherwise, if this is a 'save' operation (isSave==1), then data
    ** is copied from pInMemory to pFile.  Set the variables pFrom and
    ** pTo accordingly. */
    pFrom = (isSave ? pInMemory : pFile);
    pTo   = (isSave ? pFile     : pInMemory);

    /* Set up the backup procedure to copy from the "main" database of 
    ** connection pFile to the main database of connection pInMemory.
    ** If something goes wrong, pBackup will be set to NULL and an error
    ** code and message left in connection pTo.
    **
    ** If the backup object is successfully created, call backup_step()
    ** to copy data from pFile to pInMemory. Then call backup_finish()
    ** to release resources associated with the pBackup object.  If an
    ** error occurred, then an error code and message will be left in
    ** connection pTo. If no error occurred, then the error code belonging
    ** to pTo is set to SQLITE_OK.
    */
    pBackup = sqlite3_backup_init(pTo, "main", pFrom, "main");
    if( pBackup ){
      (void)sqlite3_backup_step(pBackup, -1);
      (void)sqlite3_backup_finish(pBackup);
    }
    rc = sqlite3_errcode(pTo);
  }

  /* Close the database connection opened on database file zFilename
  ** and return the result of this function. */
  (void)sqlite3_close(pFile);
  return rc;
}

int main(int argc, char*argv[]) {
	init();
	writeToDatabase();
	//readFromDatabase();
	closeDatabase();
	return 0;
}



