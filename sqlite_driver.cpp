#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <unordered_map>

#include <semaphore.h>
#include <sqlite3.h>
#include <unistd.h>

#include "sqlite_driver.hpp"
#include "spacetypes.h"

/**
 * Reads chosen database and begins the process to dump it to a csv file.
 */
int dumpDb(DataType dt) {
	sem_wait(&dumpMutex);
	tableMatch = dt;
	char* zErrMsg = 0;
	int rc = sqlite3_exec(db, "SELECT * FROM " + typeMap[dt], writeToCsv, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		std::cout << "Failed to open database\n";
		std::cout << zErrMsg;
		sqlite3_free(zErrMsg);
		return -1;
	}
	return 0;
}


/**
 * Callback method - Reads all data from a certain table and dumps it to a csv
 * file.
 *
 * Method is expected to have thread safe access to tableMatch.
 */
static char* writeToCsv(void *NotUsed, int argc, char **argv, char **azColName) {
	// Jump column. This is the final column, and where we insert the new line.
	std::string jCol;
	std::string logName;
	std::ofstream log(typeMap[tableMatch] + ofName);
	int numCols = -1; 
	switch (tableMatch) {
		case ACS:
			jCol = "sun_sensor_dark";
			numCols = ACS_COL;
			break;
		case POWER:
			jCol = "future_power";
			numCols = POWER_COL;
			break;
		case BEAM_STEERING:
			jCol = "phase_beam_16";
			numCols = BS_COL;
			break;
		default:
			std::perror("Attempting to log unknown subsystem");
			return
	}

	sem_post(&dumpMutex);

	for (int i = 0; i < numCols; ++i) {
		fout << azColName[i] << ",";
	}

	fout << "\n";

	for (int i = 0; i < argc; ++i) {
		fout << argv[i];
		if (std::strcmp(azColName[i], argv[i]) != 0) {
			fout << ",";
		} else {
			fout << "\n";
		}
	}

	log.close();
}


int writeDb(DataType dt, void* data) {
	char* zErrMsg = 0;
	int rc;

	switch (dt) {
		case ACS:
			struct AcsData = (AcsData) (*data);
			int rc = sqlite3_exec(db, "INSERT INTO " + typeMap[ACS] 
					+ " VALUES ( " 
					+ data.mag_x
					+ data.mag_y
					+ data.mag_z
					+ data.gyro_x
					+ data.gyro_y
					+ data.gyro_z
					+ data.light_sensor_1
					+ data.light_sensor_2
					+ data.light_sensor_3
					+ data.light_sensor_4
					+ data.light_sensor_4
					+ data.light_sensor_5
					+ data.light_sensor_6
					+ data.light_sensor_7
					+ data.light_sensor_8
					+ " )", writeCallback, 0, &zErrMsg);
			break;
		case POWER:
			// TODO: Insert power vals
			struct PowerData = (PowerData) (*data);
			break;
		case BEAM_STEERING:
			// TODO: Insert beam steering vals
			struct BeamSteeringData = (BeamSteeringData) (*data);
			break;
		default:
			rc = -1;
	}

	int rc = sqlite3_exec(db, "INSERT INTO power VALUES (null, 4.5, 6.7)", writeCallback, 0, &zErrMsg);

	if (rc != SQLITE_OK) {
		std::cout << "Failure to write command\n";
		std::cout << zErrMsg;
		sqlite3_free(zErrMsg);
		return -1;
	}
	return 0;
}


static int writeCallback(void *NotUsed, int argc, char **argv, char **azColName) {
	// We really don't give a damn that it finished. It just needs to do
	// it's job. 
	// SQlite3 requires a callback method though, so here we are.
	return 1;
}

/************
 * LEGACY SQL FUNCTIONS -- end
 */


void init() {
	// Set up data map
	typeMap[ACS] = "acs";
	typeMap[POWER] = "power";
	typeMap[BEAM_STEERING] = "beam_steering";

	// Set up table vars
	ofName = "_log.csv";
	tableMatch = NONE;
	numCol = -1;

	// Init semaphore
	sem_init(&dumpMutex, 0, 252);

	// Open the SQLite database 
	int rc = sqlite3_open_v2("octo", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
	if (rc != SQLITE_OK) {
		std::cout << "There is an issue with SQLite. Exiting.\n";
		exit(1);
	}

	// Creates table if it doesn't exist
	const char* acsTable = "CREATE TABLE IF NOT EXISTS " + typeMap[ACS] +" ( \
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
				sun_sensor_dark BOOL);";

	const char* beamSteeringTable = "CREATE TABLE IF NOT EXISTS " + typeMap[BEAM_STEERING] +" ( \
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
	
	const char* powerTable = "CREATE TABLE IF NOT EXISTS " + typeMap[POWER] + " ( \
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

void cleanUp() {
	sem_destroy(dumpMutex);
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
	if(rc==SQLITE_OK){

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
		if(pBackup){
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
	cleanUp();
	return 0;
}



