// Copyright 2018 UML SPACEHAUC
#include "./lumberjack.h"

#include <signal.h>
#include <sqlite3.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include "./publisher.h"
#include "./spacetypes.h"
#include "./subscriber.h"


void end(int signo) {
        execute = false;
        // Let the other guys finish their thing
        while (!dumpIsFinished && !logIsFinished) {}
}


char* dumpDb(DataType dt) {
        struct DumpInfo dInfo = {
                .firstRun = true,
                .outName = typeMap[dt] + ofName,
        };
        char* zErrMsg = 0;

        std::remove(dInfo.outName.c_str());

	if (dt == NONE) {
		return msg;
	}

        if (dt == ALL) {
                dumpDb(ACS);
                dumpDb(POWER);
                dumpDb(BEAM_STEERING);
                return msg;
        }

        std::string query = "SELECT * FROM " + typeMap[dt];
        int rc = sqlite3_exec(
                        db,
                        query.c_str(),
                        writeToCsv,
                        reinterpret_cast<void*>(&dInfo),
                        &zErrMsg);

        if (rc != SQLITE_OK) {
                std::cout << "Failed to open database " + typeMap[dt] + " \n";
                std::cout << zErrMsg;
                sqlite3_free(zErrMsg);
                return msg;
        }
        return msg;
}


static int writeToCsv(void *data, int argc, char **argv, char **azColName) {
        DumpInfo* dInfo = (reinterpret_cast<DumpInfo*>(data));
        std::ofstream log(dInfo -> outName, std::ios_base::app);
        if (dInfo -> firstRun) {
                dInfo -> firstRun = false;
                for (int i = 0; i < argc; ++i) {
                        log << azColName[i];
                        if (i < argc - 1) {
                                log << ",";
                        } else {
                                log << "\n";
                        }
                }
        }

        for (int i = 0; i < argc; ++i) {
                log << argv[i];
                if (i < argc - 1) {
                        log << ",";
                } else {
                        log << "\n";
                }
        }

        log.close();
        return 0;
}


char* writeDb(DataType dt, void* data) {
        char* zErrMsg = 0;
        int rc;

        // Each switch section needs to be scoped to avoid cross-initialization
        // error.
        switch (dt) {
                case ACS:
                        {
                                struct AcsData aData = *((AcsData*) data); // NOLINT
                                std::string aQuery = "INSERT INTO "
                                        + typeMap[ACS]
                                        + " VALUES ( "
                                        + " null " + ","
                                        + std::to_string(aData.mag_x)
                                        + ", "
                                        + std::to_string(aData.mag_y)
                                        + ", "
                                        + std::to_string(aData.mag_z)
                                        + ", "
                                        + std::to_string(aData.gyro_x)
                                        + ", "
                                        + std::to_string(aData.gyro_y)
                                        + ", "
                                        + std::to_string(aData.gyro_z)
                                        + ", "
                                        + std::to_string(aData.light_sensor_1)
                                        + ", "
                                        + std::to_string(aData.light_sensor_2)
                                        + ", "
                                        + std::to_string(aData.light_sensor_3)
                                        + ", "
                                        + std::to_string(aData.light_sensor_4)
                                        + ", "
                                        + std::to_string(aData.light_sensor_4)
                                        + ", "
                                        + std::to_string(aData.light_sensor_5)
                                        + ", "
                                        + std::to_string(aData.light_sensor_6)
                                        + ", "
                                        + std::to_string(aData.light_sensor_7)
                                        + ", "
                                        + std::to_string(aData.light_sensor_8)
                                        + " )";
                                rc = sqlite3_exec(
                                                db,
                                                aQuery.c_str(),
                                                NULL,
                                                0,
                                                &zErrMsg);
                        }
                        break;
                case BEAM_STEERING:
                        {
                                struct BeamSteeringData bData =
                                        *((BeamSteeringData*) data); // NOLINT
                                std::string bQuery = "INSERT INTO "
                                        + typeMap[BEAM_STEERING]
                                        + " VALUES ( "
                                        + " null "
                                        + ","
                                        + std::to_string(bData.phase_beam_1)
                                        + ", "
                                        + std::to_string(bData.phase_beam_2)
                                        + ", "
                                        + std::to_string(bData.phase_beam_3)
                                        + ", "
                                        + std::to_string(bData.phase_beam_4)
                                        + ", "
                                        + std::to_string(bData.phase_beam_5)
                                        + ", "
                                        + std::to_string(bData.phase_beam_6)
                                        + ", "
                                        + std::to_string(bData.phase_beam_7)
                                        + ", "
                                        + std::to_string(bData.phase_beam_8)
                                        + ", "
                                        + std::to_string(bData.phase_beam_9)
                                        + ", "
                                        + std::to_string(bData.phase_beam_10)
                                        + ", "
                                        + std::to_string(bData.phase_beam_11)
                                        + ", "
                                        + std::to_string(bData.phase_beam_12)
                                        + ", "
                                        + std::to_string(bData.phase_beam_13)
                                        + ", "
                                        + std::to_string(bData.phase_beam_14)
                                        + ", "
                                        + std::to_string(bData.phase_beam_15)
                                        + ", "
                                        + std::to_string(bData.phase_beam_16)
                                        + " )";
                                rc = sqlite3_exec(
                                                db,
                                                bQuery.c_str(),
                                                NULL,
                                                0,
                                                &zErrMsg);
                        }
                        break;
                case POWER:
                        {
                                struct PowerData pData = *((PowerData*) data); // NOLINT
                                std::string pQuery = "INSERT INTO "
                                        + typeMap[POWER]
                                        + " VALUES ( "
                                        + " null " + ","
                                        + std::to_string(pData.net_power) + ", "
                                        + std::to_string(pData.future_power)
                                        + " )";
                                rc = sqlite3_exec(
                                                db,
                                                pQuery.c_str(),
                                                NULL,
                                                0,
                                                &zErrMsg);
                        }
                        break;
                default:
                        rc = SQLITE_OK - 1;
        }
        if (rc != SQLITE_OK) {
                std::cout << "Failure to write command\n";
                std::cout << zErrMsg;
                sqlite3_free(zErrMsg);
                return msg;
        }
        return msg;
}


void initDb() {
        // Set up data map
        typeMap[ACS] = "acs";
        typeMap[POWER] = "power";
        typeMap[BEAM_STEERING] = "beam_steering";
	typeMap[NONE] = "none";
	typeMap[ALL] = "all";

        // Open the SQLite database
        int rc = sqlite3_open_v2(
                        "octo",
                        &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                        nullptr);

        if (rc != SQLITE_OK) {
                std::cout << "There is an issue with SQLite. Exiting.\n";
                exit(1);
        }

        // Creates table if it doesn't exist
        std::string acsTable = "CREATE TABLE IF NOT EXISTS "
                + typeMap[ACS] +
                "( id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "mag_x FLOAT,"
                "mag_y FLOAT,"
                "mag_z FLOAT,"
                "gyro_x FLOAT,"
                "gyro_y FLOAT,"
                "gyro_z FLOAT,"
                "light_sensor_1 FLOAT,"
                "light_sensor_2 FLOAT,"
                "light_sensor_3 FLOAT,"
                "light_sensor_4 FLOAT,"
                "light_sensor_5 FLOAT,"
                "light_sensor_6 FLOAT,"
                "light_sensor_7 FLOAT,"
                "light_sensor_8 FLOAT,"
                "sun_sensor_dark BOOL);";

        std::string beamSteeringTable = "CREATE TABLE IF NOT EXISTS "
                + typeMap[BEAM_STEERING] +
                "( id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "phase_beam_1 FLOAT,"
                "phase_beam_2 FLOAT,"
                "phase_beam_3 FLOAT,"
                "phase_beam_4 FLOAT,"
                "phase_beam_5 FLOAT,"
                "phase_beam_6 FLOAT,"
                "phase_beam_7 FLOAT,"
                "phase_beam_8 FLOAT,"
                "phase_beam_9 FLOAT,"
                "phase_beam_10 FLOAT,"
                "phase_beam_11 FLOAT,"
                "phase_beam_12 FLOAT,"
                "phase_beam_13 FLOAT,"
                "phase_beam_14 FLOAT,"
                "phase_beam_15 FLOAT,"
                "phase_beam_16 FLOAT);";

        std::string powerTable = "CREATE TABLE IF NOT EXISTS "
                + typeMap[POWER] +
                " ( id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "net_power FLOAT,"
                "future_power FLOAT);";

        sqlite3_exec(db, acsTable.c_str(), nullptr, nullptr, nullptr);
        sqlite3_exec(db, beamSteeringTable.c_str(), nullptr, nullptr, nullptr);
        sqlite3_exec(db, powerTable.c_str(), nullptr, nullptr, nullptr);
}


void disconnectDb() {
        loadOrSaveDb(db, "octo", 1);
        sqlite3_close(db);
}


void connectDb() {
	loadOrSaveDb(db, "octo", 0);
}


int loadOrSaveDb(sqlite3 *pInMemory, const char *zFilename, int isSave) {
        int rc;
        sqlite3 *pFile;
        sqlite3_backup *pBackup;
        sqlite3 *pTo;
        sqlite3 *pFrom;
        rc = sqlite3_open(zFilename, &pFile);
        if (rc == SQLITE_OK) {
                // Check to see if using in memory database.
                // Will load and save accordingly.
                pFrom = (isSave ? pInMemory : pFile);
                pTo   = (isSave ? pFile     : pInMemory);

                // Set up backup
                pBackup = sqlite3_backup_init(pTo, "main", pFrom, "main");
                if (pBackup) {
                        (void) sqlite3_backup_step(pBackup, -1);
                        (void) sqlite3_backup_finish(pBackup);
                }
                rc = sqlite3_errcode(pTo);
        }

        (void) sqlite3_close(pFile);
        return rc;
}


void* manageDump(void* arg) {
	// Ignore signals
	sigset_t mask;
	sigfillset(&mask);
	sigprocmask(SIG_SETMASK, &mask, NULL);
	DataType dt;

	if (db == NULL) {
		connectDb();
		initDb();
		disconnectDb();
	}

	while (execute) {
		dt = dumpScriber -> get_data();
		connectDb();
		dumpDb(dt);
		disconnectDb();
	}
	delete dumpScriber;
	dumpIsFinished = true;
	return NULL;
}


void* manageLog(void* arg) {
	// Ignore signals
	sigset_t mask;
	sigfillset(&mask);
	sigprocmask(SIG_SETMASK, &mask, NULL);

	if (db == NULL) {
		connectDb();
		initDb();
		disconnectDb();
	}

	LogData ld;
	while (execute) {
		ld = logScriber -> get_data();
		connectDb();
		writeDb(ld.dt, ld.data);
		disconnectDb();
	}
	delete logScriber;
	logIsFinished = true;
	return NULL;
}


int main(int argc, char*argv[]) {
	int key;
	if (argc > 1) key = std::atoi(argv[1]);
	else
		exit(5);

	struct sigaction handler;
	pthread_t octoSubT;

	handler.sa_handler = end;

	if (pthread_create(&octoSubT,
				NULL,
				subscriber_manager::wait_for_data,
				NULL)) {
		exit(1);
	}

	dumpScriber = new subscriber<DataType>("dumpMod", key);
	logScriber = new subscriber<LogData>("logMod", key);

	if (pthread_create(&dumpManager,
				NULL,
				manageDump,
				NULL)) {
		exit(1);
	}

	if (pthread_create(&logManager,
				NULL,
				manageLog,
				NULL)) {
		exit(1);
	}

	sigaction(SIGINT, &handler, 0);
	sigaction(SIGTERM, &handler, 0);

	pause();
	return 0;
}

