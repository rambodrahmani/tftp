/**
 * File: tftp_server.c
 *       TFTP Server Source File.
 *
 *       Compile using the Provided Makefile.
 *
 *       Execute using
 *          $ ./bin/tftp_server <port> <base directory>
 * 
 * Author: Rambod Rahmani <rambodrahmani@autistici.org>
 *         Created on 21/10/2019.
 */

#include "../include/tftp_server.h"

int createUDPSocket(int port)
{
    // socket to be returned
    int sockfd;

    // server address
    struct sockaddr_in serv_addr;

    // create IPv4 UDP unbound socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // check if the socket was correctly created
    if (sockfd < 0)
    {
        // if not, print a warning error message
        fprintf(stdout, "Error while creating listener socket.");

        // return with errors
        return -1;
    }

    // zero out server address struct
    memset(&serv_addr, 0, sizeof(serv_addr));

    // fill server address struct: use IPv4 address family
    serv_addr.sin_family = AF_INET;

    // bind to all local interfaces
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // set network port: port numbers below 1024 are privileged ports
    serv_addr.sin_port = htons(port);

    //
    int bound = bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));

    // return the initialized socket
    return sockfd;
}

void receive_packets(int socket)
{

}

/**
 * Entry point.
 *
 * @param  argc  command line arguments counter.
 * @param  argv  command line arguments.
 *
 * @return       execution exit code.
 */
int main(int argc, char * argv[])
{
    // check if 3 arguments were not provided
    if (argc != 3)
    {
        // if so, print a warning error message
        fprintf(stderr, "Invalid number of arguments.\n"
                        "Usage: tftp_server <port> <base directory>.\n");

        // return with errors
        return -1;
    }

    // set listener server port
    int port = atoi(argv[1]);

    // create listener UDP server
    int listener = createUDPSocket(port);

    // return with no errors
    return 0;
}

