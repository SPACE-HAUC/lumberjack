#ifndef __SQLITE_DRIVER_H
#define __SQLITE_DRIVER_H

#include <sqlite3.h>

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

/**
 * Callback method for when read is completed. 
 * @return Just 0.
 */
static int readCallback(void *NotUsed, int argc, char **argv, char **azColName);

/**
 * Begins reading values from the database. 
 * Reads will returned in the readCallback method.
 * @return 0 on success, -1 on failure.
 */
static int writeCallback(void *NotUsed, int argc, char **argv, char **azColName);

/**
 * Writes values to the database. 
 * Returns primary key of inserted values in writeCallback method.
 * @return 0 on success, -1 on failure.
 */
int writeToDatabase();

/**
 * Begins reading from database.
 * Read happens in readCallback method. 
 * @return 0 on success, -1 on failure.
 */
int readFromDatabase();

/**
 * Closes reference to database.
 */
void closeDatabase();

#endif  // __SQLITE_DRIVER_H
