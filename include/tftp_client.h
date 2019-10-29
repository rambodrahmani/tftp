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
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "common.h"

/**
 * Char array used for formatted log messages.
 */
char log_message[1024];

/**
 * TFTP Server IP Address.
 */
char *server_ip;

/**
 * TFTP Server Port.
 */
int server_port;

/**
 * TFTP Server Address Struct.
 */
struct sockaddr_in serv_addr;

/**
 * Client Socket.
 */
int cli_socket;

/**
 * TFTP Transfer Mode.
 */
char transfer_mode[4];

/**
 * Implements the execution main loop.
 */
void main_loop();

/**
 * Prints the help menu containing the available commands for the TFTP Client.
 */
void print_help_menu();

/**
 * Prints the prompt character on a new line.
 */
void print_prompt();

/**
 * Sets the transfer mode based on the given input command.
 */
void set_transfer_mode();

/**
 * Retrieves parameters for the !get command and transfers the file from the
 * TFTP Server to the Client.
 */
void get_file();

/**
 * Sends the RRQ for the given file name using the provided socket. The transfer
 * mode is the one globally set using the !mode command.
 *
 * @param  sockfd     client socket file descriptor;
 */
void send_RRQ(char * file_name);

#endif

