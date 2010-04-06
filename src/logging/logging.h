/**
 * @file logging.h
 * @author Julian Kranz
 * @date 02.03.2010
 * @version $Rev$
 *
 * @brief This file defines all exported data structures, functions, constants and variables pertaining to
 * the logging subsystem.
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

#ifndef LOGGING_H_
#define LOGGING_H_

/**
 * @brief This constant is used to define the log level to be set if no logging is wanted.
 * 
 * \latexonly \\ \\ \endlatexonly
 * \em Detailed \em description \n
 * This constant is used to define the log level to be set if no logging is wanted. It should
 * in general not be used as a log level for log messages.
 */
#define LOGGING_LEVEL_NONE 0

/**
 * @brief This constant is used to define the log level to be set if at most messages for critical
 * errors are wanted to be logged.
 */
#define LOGGING_LEVEL_CRITICAL 1

/**
 * @brief This constant is used to define the log level to be set if at most error messages are
 * wanted to be logged.
 */
#define LOGGING_LEVEL_ERROR 100

/**
 * @brief This constant is used to define the log level to be set if at most warning messages are
 * wanted to be logged.
 */
#define LOGGING_LEVEL_WARNING 1000

/**
 * @brief This constant is used to define the log level to be set if at most info messages are
 * wanted to be logged.
 */
#define LOGGING_LEVEL_INFO 10000

/**
 * @brief This constant is used to define the log level to be set if as most debug messages are
 * wanted to be logged.
 */
#define LOGGING_LEVEL_DEBUG 50000

/**
 * @brief This constant is used to define the log level to be set if all messages are
 * wanted to be logged.
 * 
 * \latexonly \\ \\ \endlatexonly
 * \em Detailed \em description \n
 * This constant is used to define the log level to be set if all messages are
 * wanted to be logged. It should in general not be used as a log level for log messages.
 */
#define LOGGING_LEVEL_ALL 65535

extern void logging_init();
extern void logging_free();
extern void logging_set_level(unsigned short level);
extern unsigned short logging_get_level();
extern void logging_set_destination(FILE *destination);
extern FILE *logging_get_destination();
extern void logging_set_timestamp_printed(char timestamp_printed);
extern void logging_log(const char *module, unsigned short level, const char *message, ...);
extern FILE *logging_aquire_fd(unsigned short level);
extern void logging_release_fd(unsigned short level);

#endif /* LOGGING_H_ */
