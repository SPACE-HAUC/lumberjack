#ifndef __SQLITE_DRIVER_H
#define __SQLITE_DRIVER_H

#include <map>

#include <semaphore.h>
#include <string>
#include <sqlite3.h>
#include <unistd.h>


/**
 * Number of columns in each subsystem.
 */
#define ACS_COL 	16
#define POWER_COL 	3
#define BS_COL 		17


/**
 * An enum denoting all of the possible tables the user can read from.
 * NONE is added as an error state in the case an invalid value is passed in.
 */
enum DataType {
	ACS,
	POWER,
	BEAM_STEERING,
	NONE
};


char* msg = 0;

/**
 * Location and name of log file.
 * When writing to, this will be appendended to the name of the team that is
 * dumping it's logs. e.g. ACS_log.csv
 */
std::string ofName;


/**
 * Map from each enum to a string representing them. 
 * This is initialized in {@link init}.
 */
std::map<DataType, std::string> typeMap;


/**
 * Var used by read callback to determine the table it is reading from.
 * This is used to match on the final column name as a way of adding a
 * new line at the end of the CSV it is writing to.
 */
DataType tableMatch;


/**
 * Lock used to acquire {@link tableMatch} for writing to. 
 */
sem_t dumpMutex;


/**
 * Global reference to SQLite database.
 */
static sqlite3 *db;


/**
 * Initializes the SQLite Database.
 * Attempting to use any database methods before calling init will cause a 
 * SIGSEGV.
 */
void init();

char* dumpDb(DataType dt);
static int writeToCsv(void *NotUsed, int argc, char **argv, char **azColName);

char* writeDb(DataType dt, void* data);
static int writeCallback(void *NotUsed, int argc, char **argv, char **azColName);


void cleanUp();

/**
 * Closes reference to database.
 */
void closeDatabase();

/*
** This function is used to load the contents of a database file on disk 
** into the "main" database of open database connection pInMemory, or
** to save the current contents of the database opened by pInMemory into
** a database file on disk. pInMemory is probably an in-memory database, 
** but this function will also work fine if it is not.
**
** Parameter zFilename points to a nul-terminated string containing the
** name of the database file on disk to load from or save to. If parameter
** isSave is non-zero, then the contents of the file zFilename are 
** overwritten with the contents of the database opened by pInMemory. If
** parameter isSave is zero, then the contents of the database opened by
** pInMemory are replaced by data loaded from the file zFilename.
**
** If the operation is successful, SQLITE_OK is returned. Otherwise, if
** an error occurs, an SQLite error code is returned.
*/
// NEEDS TO BE CALLED AFTER EACH WRITE TO ENSURE DATA IS SAVED
// #REFACTORME
int loadOrSaveDb(sqlite3 *pInMemory, const char *zFilename, int isSave);

#endif  // __SQLITE_DRIVER_H
