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

