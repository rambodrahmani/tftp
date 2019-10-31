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
 * TFTP Transfer Mode.
 */
char transfer_mode[10];

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
 * @param  sockfd      client socket file descriptor;
 * @param  cli_socket  the socket to be used to send the packet.
 */
void send_RRQ(int cli_socket, char * file_name);

/**
 * Sends the ACK packet for the given block number.
 *
 * @param  received   block number;
 * @param  cli_socket the socket to be used to send the packet.
 */
void send_ACK(int cli_socket, uint16_t block_number);

#endif

