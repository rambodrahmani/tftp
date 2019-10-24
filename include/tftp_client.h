/**
 * File: tftp_client.h
 *       TFTP Client Header File.
 * 
 * Author: Rambod Rahmani <rambodrahmani@autistici.org>
 *         Created on 21/10/2019.
 */

#ifndef TFTP_CLIENT_H
#define TFTP_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "common.h"

/**
 * TFTP Server IP Address.
 */
char *server_ip;

/**
 * TFTP Server Port.
 */
int      server_port;

/**
 * Implements the execution main loop.
 */
void main_loop();

/**
 * Prints the help menu containing the available commands for the TFTP Client.
 */
void print_help_menu();

#endif

