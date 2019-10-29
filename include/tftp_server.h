/**
 * File: tftp_server.h
 *       TFTP Server Header File.
 * 
 * Author: Rambod Rahmani <rambodrahmani@autistici.org>
 *         Created on 21/10/2019.
 */

#ifndef TFTP_SERVER_H
#define TFTP_SERVER_H

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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
 * TFTP Server Base Directory.
 */
char *base_dir;

/**
 * Listener UDP Server.
 */
int listener;

/**
 * Creates a listener socket having domain AF_INET and type SOCK_DGRAM on the
 * given port and binds it to the address and port specified.
 *
 * @param  port  server socket port;
 *
 * @return       the created UDP socket or < 0 in case of error.
 */
int createUDPSocket(int port);

/**
 * Implements the main loop with the UDP listener server waiting for incoming
 * packets.
 */
void listen_for_packets();

/**
 * Handles invalid opcodes received from the TFTP Client. An error message
 * (opcode = 5) is sent to the client for illegal TFTP operation (error code =
 * 4).
 *
 * @param  cli_addr  TFTP Client socket address.
 */
void handle_invalid_opcode(struct sockaddr cli_addr);

/**
 * Handles requested file form client not found. An error message (opcode = 5)
 * is sent to the client for File not found (error code = 1:).
 *
 * @param  cli_addr  TFTP Client socket address.
 */
void handle_file_not_found(struct sockaddr cli_addr);

#endif

