/**
 * File: common.h
 *       Common functions and data structures for the Server and the Client.
 * 
 * Author: Rambod Rahmani <rambodrahmani@autistici.org>
 *         Created on 24/10/2019.
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

/**
 * Set this to 1 to enable debugging log messages.
 */
#define DEBUG 0

/**
 * Maximum transfer buffer size.
 */
#define BUFSIZE 1024

/**
 * Maximum bytes in the Data message.
 */
#define MAX 512

/**
 * Char array used for formatted log messages.
 */
char log_message[1024];

/**
 * Available types for log messages.
 */
typedef enum {
	INFO,		// info log message
	ERROR		// error log message
} LogType;

/**
 * Prints a log message having the given type with the given text message.
 * Based on the given type, a different output stream is used. The new line
 * character is automatically added after the log message.
 *
 * @param  type     the type of the log message;
 * @param  message  the text of the log message.
 */
void print_log(LogType type, const char *message);

/**
 * Custom logging method for child processes log messages in order to be able
 * to distinguish them from the parent process.
 *
 * @param  type     the type of the log message;
 * @param  message  the text of the log message.
 */
void child_log(LogType type, const char *message);

/**
 * Checks for transfer errors and eventually prints the content of errno. In
 * case of error the process is terminated.
 *
 * @param  ret   transfer result;
 * @param  info  string containing additional human readable info.
 */
void check_errno(int ret, char *info);

#endif
