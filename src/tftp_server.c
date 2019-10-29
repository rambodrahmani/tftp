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

    // associate the socket with its local address
    int bound = bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    // check if the socket bound succeeded
    if (bound < 0)
    {
        // return with errors
        return -1;
    }

    // server socket successfully started, print connection parameters
    print_log(INFO, "TFTP Server successfully started.");

    // retrieve and print server formatted IP address xxx.xxx.xxx.xxx
    char ip[17];
    inet_ntop(serv_addr.sin_family, (void *)&serv_addr.sin_addr, ip, sizeof(ip));

    // prepare log message string
    sprintf(log_message, "Server IP: %s", ip);
    print_log(INFO, log_message);

    // retrieve and print server formatted port
    sprintf(log_message, "Server Port: %d", ntohs(serv_addr.sin_port));
    print_log(INFO, log_message);

    // print base directory
    sprintf(log_message, "Base directory: %s", base_dir);
    print_log(INFO, log_message);

    // return the initialized socket
    return sockfd;
}

void listen_for_packets(int socket)
{
    // print info log message
    print_log(INFO, "Main listener loop started.");

    // incoming client socket address
    struct sockaddr cli_addr;

    // client address length
    size_t cli_size = sizeof(cli_addr);

    // received message length
    int recv_len;

    // incoming message buffer
    char buffer[BUFSIZE];

    // received file name
    char file_name[32];

    // received transfer mode
    char mode[32];

    // received opcode
    uint16_t opcode;

    // file transfer block number
    uint16_t block_number;

    // 
    pid_t fork_id;

    // infinite loop
    while(1)
    {
        // clean the buffer
        memset(buffer, 0, BUFSIZE);

        // recieve the data
        recv_len = recvfrom(socket, (char *)buffer, BUFSIZE, MSG_WAITALL,
                            &cli_addr, (socklen_t *)&cli_size);

        // check for errors
        if (recv_len <= 0)
        {
            // errors occurred, print a warning error message
            sprintf(log_message, "Error while sending ERROR message: %d", errno);
            print_log(ERROR, log_message);

            // quit with errors
            exit(-1);
        }

        // retrieve opcode
        memcpy(&opcode, (uint16_t*)&buffer, 2);
        opcode = ntohs(opcode);

        // retrieve file name
        strcpy(file_name, buffer + 2);

        // retrieve transfer mode
        strcpy(mode, buffer + 3 + strlen(file_name));

        // log received message info
        sprintf(log_message, "Received opcode: %d, file name: %s and mode: %s.",
                              opcode, file_name, mode);
        print_log(INFO, log_message);

        // the only valid opcode at this point is 1 (RRQ)
        if (opcode != 1)
        {
            // print a warning error message
            sprintf(log_message, "Received invalid opcode: %d.", opcode);
            print_log(ERROR, log_message);

            // handle invalid opcode received
            handle_invalid_opcode(socket, cli_addr);
        }
    }
}

void handle_invalid_opcode(int sockfd, struct sockaddr cli_addr)
{
    // terminating end string
    uint8_t end_string = 0;

    // response message buffer
    char buffer[BUFSIZE];

    // response message error message text
    char error_message[512];

    // set opcode (ERROR = 5)
    uint16_t opcode = htons(5);

    // set error code (4 = Illegal TFTP operation)
    uint16_t error_code = htons(4);

    // copy opcode to the transfer buffer
    memcpy(buffer, &opcode, 2);

    // copy error code to the transfer buffer
    memcpy(buffer + 2, &error_code, 2);

    // set error message text
    strcpy(error_message, "Illegal TFTP operation");

    // copy error message to the tranfer buffer
    strcpy(buffer + 2, error_message);

    // add final terminating end string to the buffer
    memcpy(buffer + strlen(error_message) + 2, &end_string, 1);
    
    // send error message to the TFTP client
    int sent_len = sendto(sockfd,
                          buffer,
                          strlen(error_message) + 4,
                          MSG_CONFIRM,
                          (const struct sockaddr *) &cli_addr,
                          sizeof(cli_addr));

    // check for errors
    if (sent_len <= 0)
    {
        // errors occurred, print a warning error message
        sprintf(log_message, "Error while sending ERROR message: %d", errno);
        print_log(ERROR, log_message);

        // quit with errors
        exit(-1);
    }

    // error message correctly sent
    print_log(INFO, "Error message correctly sent.");
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

        // set tftp server base directory
        base_dir = argv[2];
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

    // check if the listener socket was correctly created
    if (listener < 0)
    {
        // if not, print a warning log message
        print_log(ERROR, "Error while creating listener socket. Quitting.");

        // return with errors
        return -1;
    }

    // start main loop
    listen_for_packets(listener);

    // return with no errors
    return 0;
}

