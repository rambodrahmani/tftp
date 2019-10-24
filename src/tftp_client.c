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

        // HELP
        if (strncmp(command, "!help", 5) == 00)
        {
            // print help menu
            print_help_menu();
        }

        // MODE
        if (strncmp(command, "!mode", 5) == 0)
        {
            // retrieve transfer mode from input command
            char *mode;
            strncpy(mode, &command+2, 3);
            //transfer_mode = mode;
            print_log(INFO, mode);
        }

        // GET
        if (strncmp(command, "!get", 4) == 0)
        {}

        // QUIT
        if (strncmp(command, "!quit", 5) == 00)
        {
            // print an info log message
            print_log(INFO, "Quitting TFTP Client as requested.");

            // exit infinite while loop
            break;
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

    // start main loop
    main_loop();

    // return without errors
    return 0;
}

