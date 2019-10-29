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
 * Creates a listener socket having domain AF_INET and type SOCK_DGRAM on the
 * given port and binds it to the address and port specified.
 *
 * @param  port  server socket port;
 *
 * @return       the created UDP socket or < 0 in case of error.
 */
int createUDPSocket(int port);

/**
 * Implements the mail loop with the UDP listener server waiting for incoming
 * packets.
 *
 * @param  socket  the UDP socket to be used to listen for incoming packets.
 */
void listen_for_packets(int socket);

/**
 * Handles invalid opcodes received from the TFTP Client. An error message
 * (opcode = 5) is sent to the client for illegal TFTP operation (error code =
 * 4).
 *
 * @param  sockfd    the socket to be used to send the Error Message.
 * @param  cli_addr  TFTP Client socket address.
 */
void handle_invalid_opcode(int sockfd, struct sockaddr cli_addr);

#endif

