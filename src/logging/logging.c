/**
 * @file logging.c
 * @author Julian Kranz
 * @date 02.03.2010
 * @version $Rev$
 * 
 * @brief This file contains all functions, constants and variables pertaining to the logging subsystem.
 */
/*
 *  This file is part of Logging.
 *  
 *  Logging is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Logging is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Logging.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>

#ifdef HP_UX
struct tm* localtime_r(const time_t *timer, struct tm *result);
#endif

#include "logging.h"

/**
 * @brief This variable is used to save the configured log level.
 */
static unsigned short _level = 0;

/**
 * @brief This variable is used to save whether to print a timestamp.
 */
static char _timestamp_printed = 1;

/**
 * @brief This variable is used to save the file pointer used for logging.
 */
static FILE *_destination = NULL;

/**
 * @brief This mutex is used to ensure consistent log messages, even though they're printed
 * incrementally.
 */
static pthread_mutex_t *print_mutex;

/**
 * @brief This function initialises all global variables concerning the logging subsystem.
 * 
 */
void logging_init() {
	_destination = stdout;
	_level = LOGGING_LEVEL_ALL;
	print_mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(print_mutex, NULL);
}

/**
 * @brief This function frees all global variables concerning the logging subsystem.
 */
void logging_free() {
	if(_destination != NULL && _destination != stdout && _destination != stderr)
		fclose(_destination);
	pthread_mutex_destroy(print_mutex);
	free(print_mutex);
}

/**
 * @brief This function prints a timestamp to the configured log destination.
 */
static void timestamp_print() {
	time_t ltime;
	struct tm *tm = (struct tm*)malloc(sizeof(struct tm));
	
	ltime = time(NULL);
	tm = (struct tm*)localtime_r(&ltime, tm);
	
	fprintf(_destination, "%02d.%02d.%04d/%02d:%02d:%02d", tm->tm_mday, tm->tm_mon, (tm->tm_year
			+ 1900), tm->tm_hour, tm->tm_min, tm->tm_sec);
	
	free(tm);
}

/**
 * @brief This function sets the log level.
 * 
 * @param level the log level to set
 */
void logging_set_level(unsigned short level) {
	_level = level;
}

/**
 * @brief This function gets the currently configured log level.
 * 
 * @return the requested log level
 */
unsigned short logging_get_level() {
	return _level;
}

/**
 * @brief This function sets the log destination.
 * 
 * \latexonly \\ \\ \endlatexonly
 * \em Detailed \em description \n
 * This function sets the log destination. It must not be NULL.
 * 
 * @param destination the log destination to set
 */
void logging_set_destination(FILE *destination) {
	if(destination != NULL)
		_destination = destination;
}

/**
 * @brief This function gets the currently configured log destination.
 * 
 * @return the requested log destination
 */
FILE *logging_get_destination() {
	return _destination;
}

/**
 * @brief This function sets whether to print a timestamp for each log message.
 * 
 * @param timestamp_printed the boolean value whether to print a timestamp
 */
void logging_set_timestamp_printed(char timestamp_printed) {
	_timestamp_printed = timestamp_printed;
}

/**
 * @brief This function logs a message to the log destination.
 * 
 * @param module the source module of the log message
 * @param level the log level; a higher level needs a higher configured log level for the
 * message to be printed.
 * @param message the message to be printed; it may contain format controls - see printf() for
 * details.
 * @param ... additional parameters for the format string; see printf() for details.
 */
void logging_log(const char *module, unsigned short level, const char *message, ...) {	
	va_list arglist;
	va_start(arglist, message);
	
	if(level <= _level) {
		pthread_mutex_lock(print_mutex);
		
		if(level >= LOGGING_LEVEL_DEBUG)
			fprintf(_destination, "[%d:DEBUG", level);
		else if(level >= LOGGING_LEVEL_INFO)
			fprintf(_destination, "[%d:INFO", level);
		else if(level >= LOGGING_LEVEL_WARNING)
			fprintf(_destination, "[%d:WARNING", level);
		else if(level >= LOGGING_LEVEL_ERROR)
			fprintf(_destination, "[%d:ERROR", level);
		else
			fprintf(_destination, "[%d:CRITICAL", level);
		
		if(_timestamp_printed) {
			fprintf(_destination, "@");
			timestamp_print();
		}
		fprintf(_destination, "] ");
		
		fprintf(_destination, "%s: ", module);
		vfprintf(_destination, message, arglist);
		fprintf(_destination, "\n");
		
		pthread_mutex_unlock(print_mutex);
		fflush(_destination);
	}
}

/**
 * @brief This function returns a file descriptor for the caller to be able to log a message
 * in its own format.
 * 
 * \latexonly \\ \\ \endlatexonly
 * \em Detailed \em description \n
 * This function returns a file descriptor for the caller to be able to log a message
 * in its own format. For this purpose the print mutex will be locked. The caller has to
 * use logging_release_fd() to unlock it.
 * 
 * @param level the log level of the external message to be logged; a higher level needs a
 * higher configured log level for the message to be printed.
 * 
 * @return a pointer to the logging destination; in case the specified level is too high for
 * the message to be logged, NULL is returned. In that case no mutex is locked and the caller
 * must not use logging_release_fd() afterwards.
 */
FILE *logging_aquire_fd(unsigned short level) {
	if(level <= _level) {
		logging_log("Logging", level, "*** External logging started...");
		pthread_mutex_lock(print_mutex);
		return _destination;
	}
	else
		return NULL;
}

/**
 * @brief This function unlocks the print mutex after external logging took place.
 * 
 * @param level the log level of the external log message; this parameter should have the
 * same value as the one given to logging_aquire_fd() before.
 */
void logging_release_fd(unsigned short level) {
	pthread_mutex_unlock(print_mutex);
	logging_log("Logging", level, "*** External logging ended...");
}
