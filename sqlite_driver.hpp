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
