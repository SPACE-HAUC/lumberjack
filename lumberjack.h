// Copyright 2018 UML SPACEHAUC
#ifndef LUMBERJACK_H_
#define LUMBERJACK_H_

#include <semaphore.h>
#include <sqlite3.h>
#include <unistd.h>

#include <map>
#include <string>

#include "./subscriber.h"


/**
 * An enum denoting all of the possible tables the user can read from.
 * NONE is added as an error state in the case an invalid value is passed in.
 * ALL is added as a convenience type for dumping the entire database.
 *      note - Passing ALL to a write method will cause a no-op.
 */
enum DataType {
        ACS,
        POWER,
        BEAM_STEERING,
        NONE,
        ALL,
};


/**
 * Struct containing information crucial for dump. 
 */
struct DumpInfo {
        bool firstRun;
        std::string outName;
};


/**
 * Struct for containing data another party wants to log. The type, and the 
 * data.
 */
struct LogData {
        DataType dt;
        void* data;
};


/**
 * Subscriber listening for data dumping command.
 */
subscriber<DataType> *dumpScriber;


/**
 * Subscriber listening for data to put in SQLite database.
 */
subscriber<LogData> *logScriber;


/**
 * Thread managing all dumps of database.
 */
pthread_t dumpManager;


/**
 * Thread managing all logging (writing) of database.
 */
pthread_t logManager;


/**
 * Condition variable determining if logging and dumping of db should continue.
 */
volatile bool execute = true;


/**
 * Condition variable determining if all database dumping and cleanup is 
 * complete.
 */
volatile bool dumpIsFinished = false;


/**
 * Condition variable determining if all database logging and cleanup is 
 * finished.
 */
volatile bool logIsFinished = false;


/**
 * A message that we can return to after SQL messages.
 */
char* msg = 0;


/**
 * Standard ending for log file.
 */
const std::string ofName = "_log.csv";  // NOLINT


/**
 * Map from each enum to a string representing them. 
 * This is initialized in {@link init}.
 */
std::map<DataType, std::string> typeMap;


/**
 * Global reference to SQLite database.
 */
static sqlite3 *db;


/**
 * Initializes the SQLite Database.
 * Attempting to use any database methods before calling init will cause a 
 * SIGSEGV.
 */
void connectDb();


/**
 * Saves database and closes connection.
 */
void disconnectDb();


/**
 * Dumps table of type DataType to a csv file.
 */
char* dumpDb(DataType dt);


/**
 * SQLite callback method from dumpDb.
 */
static int writeToCsv(void *data, int argc, char **argv, char **azColName);


/**
 * Writes data of data DataType to database.
 */
char* writeDb(DataType dt, void* data);


/**
 * Loads DB into memory or saves to it.
 */
int loadOrSaveDb(sqlite3 *pInMemory, const char *zFilename, int isSave);


/**
 * Manages all aspects of data dumps.
 */
void* manageDump(void* arg);


/**
 * Manages all aspects of logging new data.
 */
void* manageLog(void* arg);


/**
 * Tells the listeners to fall off their curly brace and commit suicide.
 */
void end(int signo);


#endif  // LUMBERJACK_H_
