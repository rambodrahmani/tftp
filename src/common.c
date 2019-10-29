/**
 * File: common.c
 *       Common Source File.
 * 
 * Author: Rambod Rahmani <rambodrahmani@autistici.org>
 *         Created on 24/10/2019.
 */

#include "../include/common.h"

void print_log(LogType type, const char *message)
{
    // check the given type
    switch(type)
    {
        // info log message
        case INFO:
            {
                fprintf(stdout, "> %s \n", message);
                break;
            }

        // error log message
        case ERROR:
            {
                fprintf(stderr, "!> %s \n", message);
                break;
            }
    }
}

void check_errno(int ret)
{
    char log_message[1024];

    // check for errors
    if (ret <= 0)
    {
        // errors occurred, print a warning error message
        sprintf(log_message, "An unexpected error happened: errno=%d", errno);
        print_log(ERROR, log_message);

        // quit with errors
        exit(-1);
    }
}

