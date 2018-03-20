#include <cstdio>
#include <cstdlib>
#include <cstring>
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
	std::string query = "SELECT * FROM " + typeMap[dt];
	int rc = sqlite3_exec(db, query.c_str(), writeToCsv, 0, &zErrMsg);
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
static int writeToCsv(void *NotUsed, int argc, char **argv, char **azColName) {
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
			return -1;
	}

	sem_post(&dumpMutex);

	for (int i = 0; i < numCols; ++i) {
		log << azColName[i] << ",";
	}

	log << "\n";

	for (int i = 0; i < argc; ++i) {
		log << argv[i];
		if (std::strcmp(azColName[i], argv[i]) != 0) {
			log << ",";
		} else {
			log << "\n";
		}
	}

	log.close();
	return 1;
}


int writeDb(DataType dt, void* data) {
	char* zErrMsg = 0;
	int rc;

	switch (dt) {
		case ACS:
			{
				struct AcsData aData = *((AcsData*) data);
				std::string aQuery = "INSERT INTO " + typeMap[ACS] 
					+ " VALUES ( " 
					+ std::to_string(aData.mag_x) + ", " 
					+ std::to_string(aData.mag_y) + ", " 
					+ std::to_string(aData.mag_z) + ", " 
					+ std::to_string(aData.gyro_x) + ", " 
					+ std::to_string(aData.gyro_y) + ", " 
					+ std::to_string(aData.gyro_z) + ", " 
					+ std::to_string(aData.light_sensor_1) + ", " 
					+ std::to_string(aData.light_sensor_2) + ", " 
					+ std::to_string(aData.light_sensor_3) + ", " 
					+ std::to_string(aData.light_sensor_4) + ", " 
					+ std::to_string(aData.light_sensor_4) + ", " 
					+ std::to_string(aData.light_sensor_5) + ", " 
					+ std::to_string(aData.light_sensor_6) + ", " 
					+ std::to_string(aData.light_sensor_7) + ", " 
					+ std::to_string(aData.light_sensor_8)
					+ " )";
				rc = sqlite3_exec(db, aQuery.c_str(), writeCallback, 0, &zErrMsg);
			}
			break;
		case BEAM_STEERING:
			{
				struct BeamSteeringData bData = *((BeamSteeringData*) data);
				std::string bQuery = "INSERT INTO " + typeMap[POWER]
					+ " VALUES ( " 
					+ std::to_string(bData.phase_beam_1) + ", " 
					+ std::to_string(bData.phase_beam_2) + ", " 
					+ std::to_string(bData.phase_beam_3) + ", " 
					+ std::to_string(bData.phase_beam_4) + ", " 
					+ std::to_string(bData.phase_beam_5) + ", " 
					+ std::to_string(bData.phase_beam_6) + ", " 
					+ std::to_string(bData.phase_beam_7) + ", " 
					+ std::to_string(bData.phase_beam_8) + ", " 
					+ std::to_string(bData.phase_beam_9) + ", " 
					+ std::to_string(bData.phase_beam_10) + ", " 
					+ std::to_string(bData.phase_beam_11) + ", " 
					+ std::to_string(bData.phase_beam_12) + ", " 
					+ std::to_string(bData.phase_beam_13) + ", " 
					+ std::to_string(bData.phase_beam_14) + ", " 
					+ std::to_string(bData.phase_beam_15) + ", " 
					+ std::to_string(bData.phase_beam_16) 
					+ " )";
				rc = sqlite3_exec(db, bQuery.c_str(), writeCallback, 0, &zErrMsg);
			}
			break;
		case POWER:
			{
				struct PowerData pData = *((PowerData*) data);
				std::string pQuery = "INSERT INTO " + typeMap[POWER]
					+ " VALUES ( " 
					+ std::to_string(pData.net_power) + ", " 
					+ std::to_string(pData.future_power)
					+ " )";
				rc = sqlite3_exec(db, pQuery.c_str(), writeCallback, 0, &zErrMsg);
			}
			break;
		default:
			rc = -1;
	}

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

void init() {
	// Set up data map
	typeMap[ACS] = "acs";
	typeMap[POWER] = "power";
	typeMap[BEAM_STEERING] = "beam_steering";

	// Set up table vars
	ofName = "_log.csv";
	tableMatch = NONE;

	// Init semaphore
	sem_init(&dumpMutex, 0, 252);

	// Open the SQLite database 
	int rc = sqlite3_open_v2("octo", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
	if (rc != SQLITE_OK) {
		std::cout << "There is an issue with SQLite. Exiting.\n";
		exit(1);
	}

	// Creates table if it doesn't exist
	std::string acsTable = "CREATE TABLE IF NOT EXISTS " + typeMap[ACS] + " ( \
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

	std::string beamSteeringTable = "CREATE TABLE IF NOT EXISTS " + typeMap[BEAM_STEERING] +" ( \
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

	std::string powerTable = "CREATE TABLE IF NOT EXISTS " + typeMap[POWER] + " ( \
				  id INTEGER PRIMARY KEY AUTOINCREMENT,\
				  net_power FLOAT,\
				  future_power FLOAT);";

	sqlite3_exec(db, acsTable.c_str(), nullptr, nullptr, nullptr);
	sqlite3_exec(db, beamSteeringTable.c_str(), nullptr, nullptr, nullptr);
	sqlite3_exec(db, powerTable.c_str(), nullptr, nullptr, nullptr);
}

void closeDatabase() {
	loadOrSaveDb(db, "octo", 1);
	sqlite3_close(db);
}

void cleanUp() {
	sem_destroy(&dumpMutex);
}


int loadOrSaveDb(sqlite3 *pInMemory, const char *zFilename, int isSave){
	int rc;                   
	sqlite3 *pFile;           
	sqlite3_backup *pBackup;  
	sqlite3 *pTo;             
	sqlite3 *pFrom;           
	rc = sqlite3_open(zFilename, &pFile);
	if (rc==SQLITE_OK) {
		// Check to see if using in memory database. 
		// Will load and save accordingly.
		pFrom = (isSave ? pInMemory : pFile);
		pTo   = (isSave ? pFile     : pInMemory);

		// Set up backup
		pBackup = sqlite3_backup_init(pTo, "main", pFrom, "main");
		if(pBackup){
			(void)sqlite3_backup_step(pBackup, -1);
			(void)sqlite3_backup_finish(pBackup);
		}
		rc = sqlite3_errcode(pTo);
	}

	(void) sqlite3_close(pFile);
	return rc;
}

int main(int argc, char*argv[]) {
	init();
	closeDatabase();
	cleanUp();
	return 0;
}

