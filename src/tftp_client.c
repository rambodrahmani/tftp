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
            // retrieve transfer mode from stdin
            scanf("%s", command);

            // check if the provided mode is valid
            if ((strncmp(command, "bin", 3) == 0) || 
                (strncmp(command, "txt", 3) == 0))
            {
                // store transfer mode
                strncpy(transfer_mode, command, 3);

                // prepare log message
                char log_message[33];
                sprintf(log_message, "Transfer mode set correctly: %s.",
                transfer_mode);

                // print log message
                print_log(INFO, log_message);
            }
            else
            {
                print_log(ERROR, "Invalid transfer mode. Only txt and bin modes"
                                 " are available options.");
            }

            // print prompt char and wait for a new command
            print_prompt();
        }
        else if (strncmp(command, "!get", 4) == 0)       // GET
        {}
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

