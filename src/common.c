/**
 * File: common.c
 *	 Common Source File.
 * 
 * Author: Rambod Rahmani <rambodrahmani@autistici.org>
 *	   Created on 24/10/2019.
 */

#include "../include/common.h"

/**
 * Info log messages are preceded by the starting character '>' while error log
 * messages are preceded by the starting character '!>'.
 */
void print_log(LogType type, const char *message)
{
	switch (type) {
	case INFO:
		{
			fprintf(stdout, "> %s \n", message);
			break;
		}

	case ERROR:
		{
			fprintf(stderr, "!> %s \n", message);
			break;
		}
	}
}

/**
 * Info log messages are preceded by the starting character '-->' while error
 * log messages are preceded by the starting character '--!>'.
 */
void child_log(LogType type, const char *message)
{
	switch (type) {
	case INFO:
		{
			fprintf(stdout, "--> %s \n", message);
			break;
		}

	case ERROR:
		{
			fprintf(stderr, "--!> %s \n", message);
			break;
		}
	}
}

void check_errno(int ret, char *info)
{
	if (ret <= 0)
	{
		sprintf(log_message,
			"An unexpected error happened: info = %s; errno = "
			"%d", info, errno);

		print_log(ERROR, log_message);

		exit(-1);
	}
}

