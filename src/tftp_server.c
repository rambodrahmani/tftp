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

void receive_packets(int socket)
{

}

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

void print_log(LogType type, const char *message)
{
    // check the given type
    switch(type)
    {
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
        print_log(ERROR, "Invalid number of arguments."
                         " Usage: tftp_server <port> <base directory>."
                         " Quitting.");

        // return with errors
        return -1;
    }

    // set listener server port
    int port = atoi(argv[1]);

    // check if the given port need root privileges
    if (port < 1024)
    {
        // check if the user has root privileges
        if (geteuid() != 0)
        {
            // if not, print a warning error message
            print_log(ERROR, "Invalid port number. To use ports lower than 1024"
                             " root privileges are needed. Quitting.");

            // and quit
            return -1;
        }
    }

    // check if the given directory path is valid
    DIR* dir = opendir(argv[2]);
    if (dir)
    {
        // directory correctly opened, it exists, just close it
        closedir(dir);
    }
    else if (ENOENT == errno)
    {
        // could not open the directory, it does not exist
        print_log(ERROR, "The provided directory path is not valid. Please"
                         " provide the path to an existing directory."
                         " Quitting.");

        // return with errors
        return -1;
    }
    else
    {
        // opendir() failed for other reasons
        print_log(ERROR, "An unexpected error happened while opening the"
                         " provided directory. Quitting.");
    }

    // create listener UDP server
    int listener = createUDPSocket(port);

    // start main loop
    receive_packets(listener);

    // return with no errors
    return 0;
}

