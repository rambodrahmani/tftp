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
    if ((strncmp(mode, "bin", 3) == 0) || (strncmp(mode, "txt", 3) == 0))
    {
        // store transfer mode
        strncpy(transfer_mode, mode, 3);

        // prepare log message
        sprintf(log_message, "Transfer mode set correctly: %s.",
                              transfer_mode);

        // print log message
        print_log(INFO, log_message);
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
    // transfer source file
    char source[512];

    // transfer destination file
    char dest[512];

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
    check_errno(recv_len);

    // retrieve server response opcode
    memcpy(&opcode, (uint16_t*)&buffer, 2);
    opcode = ntohs(opcode);

    // check the opcode
    if (opcode == 5)
    {
        // error message opcode found, print a warning error log
        sprintf(log_message, "Error: %s.", buffer + 2);
        print_log(ERROR, log_message);
    }
}

void send_RRQ(char * file_name)
{
    // final transfer buffer length
    int len = 0;

    // transfer buffer
    char buffer[BUFSIZE];

    // opcode to be used (RRQ = 1)
    uint16_t opcode = htons(2);

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
    check_errno(sent_len);
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
    strcpy(transfer_mode, "bin");

    // fill in tftp server address struct: use IPv4 address family
    serv_addr.sin_family = AF_INET;

    // set network address
    inet_pton(AF_INET, server_ip, &serv_addr.sin_addr);

    // set network port: port numbers below 1024 are privileged ports
    serv_addr.sin_port = htons(server_port);

    // create client socket descriptor
    cli_socket = socket(AF_INET, SOCK_DGRAM, 0);

    // start main loop
    main_loop();

    // return without errors
    return 0;
}

