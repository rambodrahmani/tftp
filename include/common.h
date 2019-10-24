/**
 * File: common.h
 *       Common Header File.
 * 
 * Author: Rambod Rahmani <rambodrahmani@autistici.org>
 *         Created on 24/10/2019.
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>

/**
 * Maximum incoming buffer size.
 */
#define BUFSIZE 1024

/**
 * Available type for log messages.
 */
typedef enum
{
    INFO,       // info log message
    ERROR       // error log message
} LogType;

/**
 * Prints a log message having the given type with the given text message. Based
 * on the given type, a different output stream is used. The new line character
 * is automatically added after the log message.
 *
 * @param  type     the type of the log message;
 * @param  message  the text of the log message.
 */
void print_log(LogType type, const char * message);

#endif

