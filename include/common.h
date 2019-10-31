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
#include <errno.h>
#include <stdlib.h>

/**
 * Maximum incoming buffer size.
 */
#define BUFSIZE 1024

/**
 * Maximum Data Packet buffer size.
 */
#define MAX 512

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

/**
 * Checks for transfer errors and eventually prints the content of errno.
 *
 * @param  ret   transfer result;
 * @param  info  string containing additional human readable info.
 */
void check_errno(int ret, char * info);

#endif

