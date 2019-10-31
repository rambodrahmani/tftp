/**
 * File: tftp_client.c
 *       TFTP Client Source File.
 * 
 * Author: Rambod Rahmani <rambodrahmani@autistici.org>
 *         Created on 21/10/2019.
 */

#include "../include/tftp_client.h"

void main_loop()
{
    // input command
    char command[1024];

    // print help menu
    print_help_menu();

    // infinite loop
    while (1)
    {
        // read command from stdin
        scanf("%1023s", command);

        if (strncmp(command, "!help", 5) == 0)          // HELP
        {
            // print help menu
            print_help_menu();

        }
        else if (strncmp(command, "!mode", 5) == 0)     // MODE
        {
            // retrieve and set new transfer mode
            set_transfer_mode();

            // print prompt char and wait for a new command
            print_prompt();
        }
        else if (strncmp(command, "!get", 4) == 0)       // GET
        {
            // retrieve get parameters and transfer file
            get_file();

            // print promp char and wait for a new command
            print_prompt();
        }
        else if (strncmp(command, "!quit", 5) == 0)      // QUIT
        {
            // print an info log message
            print_log(INFO, "Quitting TFTP Client as requested.");

            // exit infinite while loop
            break;
        }
        else                                            // INVALID COMMAND
        {
            // print a warning error message
            print_log(ERROR, "Invalid command.");

            // print the prompt char and wait for a new command
            print_prompt();
        }
    }
}

void print_help_menu()
{
    // print help message to STDOUT
    fprintf(stdout, "\n> Options:\n"
                    "   !mode <mode>\t\tSets the transfer mode to be used: "
                    "<txt> for text mode and <bin> for binary mode.\n"
                    "   !get <src> <dest>\tTransfers the file identified by "
                    "<src> from the server and saves it as <dst>.\n"
                    "   !quit\t\tQuit and close the client.\n"
                    "   !help\t\tPrint this help menu.\n");

    // print the prompt char and wait for a new command
    print_prompt();
}

void print_prompt()
{
    // print the prompt character to the stdout
    fprintf(stdout, "\n> ");
}

void set_transfer_mode()
{
    char mode[1024];

    // retrieve transfer mode from stdin
    scanf("%s", mode);

    // check if the provided mode is valid
    if (strncmp(mode, "bin", 3) == 0)           // OCTET
    {
        // store transfer mode
        strncpy(transfer_mode, "octet", 6);

        // print log message
        print_log(INFO, "Transfer mode correctly set to binary mode.");
    }
    else if (strncmp(mode, "txt", 3) == 0)      // NETASCII
    {
        // store transfer mode
        strncpy(transfer_mode, "netascii", 9);

        // print log message
        print_log(INFO, "Transfer mode correctly set to text mode.");
    }
    else
    {
        // print a warning error log message: invalid transfer mode provided
        print_log(ERROR, "Invalid transfer mode. Only txt and bin modes are "
                         "available options.");
    }
}

void get_file()
{
    // create client socket descriptor
    cli_socket = socket(AF_INET, SOCK_DGRAM, 0);

    // transfer source file
    char source[256];

    // transfer destination file
    char dest[256];

    // server response opcode
    int opcode;

    // retrieve source file name
    scanf("%s", source);

    // retrieve destination file name
    scanf("%s", dest);

    // print info log message
    sprintf(log_message, "Requesting %s from the TFTP Server.", source);
    print_log(INFO, log_message);

    // send RRQ request
    send_RRQ(source);

    // TFTP Server response buffer
    char buffer[BUFSIZE];

    // retrieve server address length
    int addr_len = sizeof(serv_addr);

    // receive response from TFTP Server
    int recv_len = recvfrom(cli_socket,
                            (char *)buffer,
                            BUFSIZE,
                            MSG_WAITALL,
                            (struct sockaddr *) &serv_addr,
                            (socklen_t *)& addr_len);

    // check for errors
    check_errno(recv_len, "receiving response after sending RRQ packet.");

    // retrieve server response opcode
    memcpy(&opcode, (uint16_t*)&buffer, 2);
    opcode = ntohs(opcode);

    // check the opcode for error messages
    if (opcode == 5)
    {
        // error message opcode found, print a warning error log
        sprintf(log_message, "Error: %s.", buffer + 2);
        print_log(ERROR, log_message);
    }
    else if (opcode == 3)           // check the opcode for data messages
    {
        // block number
        uint16_t block_number;

        // retrieve block number from transfer buffer
        memcpy(&block_number, (uint16_t*) &buffer[2], 2);

        // deserialize block number
        block_number = ntohs(block_number);
        
        // print info log message
        print_log(INFO, "Transferring file from the Server.");

        // open file in write mode
        FILE * dest_file = fopen(dest ,"w");

        // check if the file was correctly opened
        if (dest_file == NULL)
        {
            // if not, print a warning error log
            sprintf(log_message, "Error while opening the destination file: "
                                 "errno = %d", errno);
            print_log(ERROR, log_message);

            // exit with error
            exit(-1);
        }

        // write received buffer to the file considering that each data packet
        // has 2 bytes opcode and 2 bytes block number
        for (int i = 0; i < recv_len - 4; i++)
        {
            // skip opcode and block number and write
            fputc(buffer[i + 4], dest_file);
        }

        // send ACK packet for received block
        send_ACK(block_number);

        // until data packets of length >= 516 are sent
        while (recv_len == 516)
        {
            // receive data packet from the Server
            recv_len = recvfrom(cli_socket,
                                (char *)buffer,
                                BUFSIZE,
                                MSG_WAITALL,
                                (struct sockaddr *) &serv_addr,
                                (socklen_t *)& addr_len);

            // check for errors
            check_errno(recv_len, "receiving data packets.");

            // retrieve block number from transfer buffer
            memcpy(&block_number, (uint16_t*) &buffer[2], 2);

            // deserialize block number
            block_number = ntohs(block_number);

            if (recv_len != 516)
            {
                for(int i = 0; i < recv_len - 5; i++)
                {
                    fputc(buffer[i + 4], dest_file);
                }
			}

            // write received buffer to the file considering that each data packet
            // has 2 bytes opcode and 2 bytes block number
            for (int i = 0; i < recv_len - 4; i++)
            {
                // skip opcode and block number and write
                fputc(buffer[i + 4], dest_file);
            }

            // send ACK packet for received block
            send_ACK(block_number);
        }

        // transfer completed, shutdown socket read and write
        shutdown(cli_socket, SHUT_RDWR);

        // close the socket
        close(cli_socket);

        // close the destination file stream
        fclose(dest_file);

        // print an info log message
        sprintf(log_message, "File %s correctly downloaded. Saved in %s.",
                              source, dest);
        print_log(INFO, log_message);
   }
}

void send_RRQ(char * file_name)
{
    // final transfer buffer length
    int len = 0;

    // transfer buffer
    char buffer[BUFSIZE];

    // opcode to be used (RRQ = 1)
    uint16_t opcode = htons(1);

    // terminating end string
    uint8_t end_string = 0;

    // copy opcode to the tranfer buffer
    memcpy(buffer, &opcode, 2);

    // update transfer buffer size
    len += 2;

    // copy file name to the transfer buffer
    strcpy(buffer + len, file_name);

    // update transfer buffer size
    len += strlen(file_name);

    // add end string to the transfer buffer
    memcpy(buffer + len, &end_string, 1);

    // update transfer buffer size
    len++;

    // add tranfer mode to the transfer buffer
    strcpy(buffer + len, transfer_mode);

    // update transfer buffer size
    len += strlen(transfer_mode);

    // add final terminating end string to the buffer
    memcpy(buffer + len, &end_string, 1);

    // update transfer buffer size
    len++;

    // send RRQ to the TFTP Server
    int sent_len = sendto(cli_socket,        // client socket
                          buffer,            // transfer buffer
                          len,               // transfer buffer length
                          MSG_CONFIRM,
                          (const struct sockaddr *) &serv_addr,
                          sizeof(serv_addr));

    // check for errors
    check_errno(sent_len, "sending RRQ packet.");
}

void send_ACK(uint16_t block_number)
{
    // file transfer buffer length
    int len = 0;

    // transfer buffer
    char buffer[BUFSIZE];

    // opcode to be used (ACK = 4)
    uint16_t opcode = htons(4);

    // copy opcode to the tranfer buffer
    memcpy(buffer, &opcode, 2);

    // update transfer buffer size
    len += 2;

    // copy file name to the transfer buffer
    memcpy(buffer + len, &block_number, 2);

    // update transfer buffer size
    len += 2;

    // send ACK to the TFTP Server
    int sent_len = sendto(cli_socket,        // client socket
                          buffer,            // transfer buffer
                          len,               // transfer buffer length
                          MSG_CONFIRM,
                          (const struct sockaddr *) &serv_addr,
                          sizeof(serv_addr));

    // check for errors
    check_errno(sent_len, "sending ACK packet.");
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
                         " Usage: tftp_client <server ip> <server port>."
                         " Quitting.");

        // return with errors
        return -1;
    }

    // set server ip address
    server_ip = argv[1];

    // set server port
    server_port = atoi(argv[2]);

    // check if the given port need root privileges
    if (server_port < 1024)
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

    // set defaultr transfer mode.
    strcpy(transfer_mode, "octet");

    // fill in tftp server address struct: use IPv4 address family
    serv_addr.sin_family = AF_INET;

    // set network address
    inet_pton(AF_INET, server_ip, &serv_addr.sin_addr);

    // set network port: port numbers below 1024 are privileged ports
    serv_addr.sin_port = htons(server_port);

    // start main loop
    main_loop();

    // return without errors
    return 0;
}

