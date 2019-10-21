/**
 * File: tftp_server.h
 *       TFTP Server Header File.
 * 
 * Author: Rambod Rahmani <rambodrahmani@autistici.org>
 *         Created on 21/10/2019.
 */

#ifndef TFTP_SERVER
#define TFTP_SERVER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>

/**
 * Creates a listener socket having domain AF_INET and type SOCK_DGRAM on the
 * given port and binds it to the address and port specified.
 *
 * @param  port  server socket port;
 *
 * @return       the created UDP socket.
 */
int createUDPSocket(int port);

/**
 * Implements the mail loop with the UDP listener server waiting for incoming
 * packets.
 *
 * @param  socket  the UDP socket to be used to listen for incoming packets.
 */
void receive_packets(int socket);

#endif

