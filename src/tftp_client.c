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
	// input command buffer
	char command[1024];

	print_help_menu();
	print_prompt();

	// infinite execution loop
	while (1)
	{
		// read new command from stdin scanning 1023 chars
		scanf("%1023s", command);

		if (strncmp(command, "!help", 5) == 0)		// HELP
		{
			print_help_menu();
		}
		else if (strncmp(command, "!mode", 5) == 0)	// MODE
		{
			set_transfer_mode();
		}
		else if (strncmp(command, "!get", 4) == 0)	// GET
		{
			get_file();
		}
		else if (strncmp(command, "!quit", 5) == 0)	// QUIT
		{
			print_log(INFO, "Quitting TFTP Client as requested.");
			break;
		}
		else						// INVALID COMMAND
		{
			print_log(ERROR, "Invalid command.");
		}

		print_prompt();
	}
}

void print_help_menu()
{
	// print help message to the stdout
	fprintf(stdout, "\n> Options:\n"
		"   !mode <mode>\t\tSets the transfer mode to be used: "
		"<txt> for text mode and <bin> for binary mode.\n"
		"   !get <src> <dest>\tTransfers the file identified by "
		"<src> from the server and saves it as <dst>.\n"
		"   !quit\t\tQuit and close the client.\n"
		"   !help\t\tPrint this help menu.\n");
}

void print_prompt()
{
	fprintf(stdout, "\n> ");
}

void set_transfer_mode()
{
	char mode[1024];

	// retrieve transfer mode from stdin
	scanf("%1023s", mode);

	// check if the provided mode is valid and store it
	if (strncmp(mode, "bin", 3) == 0)		// OCTET
	{
		strncpy(transfer_mode, "octet", 6);
		print_log(INFO, "Transfer mode correctly set to binary mode.");
	}
	else if (strncmp(mode, "txt", 3) == 0)		// NETASCII
	{
		strncpy(transfer_mode, "netascii", 9);
		print_log(INFO, "Transfer mode correctly set to text mode.");
	}
	else						// INVALID MODE
	{
		print_log(ERROR,
			  "Invalid transfer mode. Only txt and bin modes are "
			  "available options.");
	}
}

void get_file()
{
	// fill in tftp server address struct: use IPv4 address family
	serv_addr.sin_family = AF_INET;

	// set network address
	inet_pton(AF_INET, server_ip, &serv_addr.sin_addr);

	// set network port: port numbers below 1024 are privileged ports
	serv_addr.sin_port = htons(server_port);

	// create client socket descriptor
	int cli_socket = socket(AF_INET, SOCK_DGRAM, 0);

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
	send_RRQ(cli_socket, source);

	// TFTP Server response buffer
	char buffer[BUFSIZE];

	// retrieve server address length
	int addr_len = sizeof(serv_addr);

	// receive response from TFTP Server
	int recv_len = recvfrom(cli_socket,
				(char *)buffer,
				BUFSIZE,
				MSG_WAITALL,
				(struct sockaddr *)&serv_addr,
				(socklen_t *) & addr_len);

	// check for errors
	check_errno(recv_len,
		    "Error while receiving response after sending RRQ packet");

	// retrieve server response opcode
	memcpy(&opcode, (uint16_t *) & buffer, 2);
	opcode = ntohs(opcode);

	// check the opcode for error messages
	if (opcode == 5) {
		// error message opcode found, print a warning error log
		sprintf(log_message, "Error: %s.", buffer + 2);
		print_log(ERROR, log_message);
	} else if (opcode == 3)	// check the opcode for data messages
	{
		// block number
		uint16_t block_number;

		// retrieve block number from transfer buffer
		memcpy(&block_number, (uint16_t *) & buffer[2], 2);

		// deserialize block number
		block_number = ntohs(block_number);

		// print info log message
		print_log(INFO, "Transferring file from the Server.");

		// open file in write mode
		FILE *dest_file = fopen(dest, "w");

		// check if the file was correctly opened
		if (dest_file == NULL) {
			// if not, print a warning error log
			sprintf(log_message,
				"Error while opening the destination file: "
				"errno = %d", errno);
			print_log(ERROR, log_message);

			// exit with error
			exit(-1);
		}
		// write received buffer to the file considering that each data packet
		// has 2 bytes opcode and 2 bytes block number
		int i = 0;
		for (i = 0; i < recv_len - 4; i++) {
			// skip opcode and block number and write
			fputc(buffer[i + 4], dest_file);
		}

		// send ACK packet for received block
		send_ACK(cli_socket, block_number);

		// until data packets of length >= 516 are sent
		while (recv_len == 516) {
			// receive data packet from the Server
			recv_len = recvfrom(cli_socket,
					    (char *)buffer,
					    BUFSIZE,
					    MSG_WAITALL,
					    (struct sockaddr *)&serv_addr,
					    (socklen_t *) & addr_len);

			// check for errors
			check_errno(recv_len,
				    "Error while receiving data packets");

			// retrieve block number from transfer buffer
			memcpy(&block_number, (uint16_t *) & buffer[2], 2);

			// deserialize block number
			block_number = ntohs(block_number);

			// write received buffer to the file considering that each data packet
			// has 2 bytes opcode and 2 bytes block number
			int i = 0;
			for (i = 0; i < recv_len - 4; i++) {
				// skip opcode and block number and write
				fputc(buffer[i + 4], dest_file);
			}

			// send ACK packet for received block
			send_ACK(cli_socket, block_number);
		}

		// transfer completed, shutdown socket read and write
		shutdown(cli_socket, SHUT_RDWR);

		// close the socket
		close(cli_socket);

		// close the destination file stream
		fclose(dest_file);

		// print an info log message
		sprintf(log_message,
			"File %s correctly downloaded. Saved in %s.", source,
			dest);
		print_log(INFO, log_message);
	}
}

void send_RRQ(int cli_socket, char *file_name)
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
	int sent_len = sendto(cli_socket,	// client socket
			      buffer,		// transfer buffer
			      len,		// transfer buffer length
			      MSG_CONFIRM,
			      (const struct sockaddr *)&serv_addr,
			      sizeof(serv_addr));

	// check for errors
	check_errno(sent_len, "Error while sending RRQ packet.");
}

void send_ACK(int cli_socket, uint16_t block_number)
{
	// file transfer buffer length
	int len = 0;

	// transfer buffer
	char buffer[BUFSIZE];

	// serialize opcode to be used (ACK = 4)
	uint16_t opcode = htons(4);

	// copy opcode to the tranfer buffer
	memcpy(buffer, &opcode, 2);

	// update transfer buffer size
	len += 2;

	// serialize block number
	block_number = htons(block_number);

	// copy file name to the transfer buffer
	memcpy(buffer + len, &block_number, 2);

	// update transfer buffer size
	len += 2;

	// send ACK to the TFTP Server
	int sent_len = sendto(cli_socket,	// client socket
			      buffer,		// transfer buffer
			      len,		// transfer buffer length
			      MSG_CONFIRM,
			      (const struct sockaddr *)&serv_addr,
			      sizeof(serv_addr));

	// check for errors
	check_errno(sent_len, "Error while sending ACK packet");
}

/**
 * Entry point.
 *
 * @param  argc  command line arguments counter.
 * @param  argv  command line arguments.
 *
 * @return  execution exit code.
 */
int main(int argc, char *argv[])
{
	// check if the server ip and port arguments were provided
	if (argc != 3) {
		print_log(ERROR, "Invalid number of arguments. "
			  "Usage: tftp_client <server ip> <server port>. "
			  "Quitting.");

		return -1;
	}

	// set server ip address and port
	server_ip = argv[1];
	server_port = atoi(argv[2]);

	// check if the given port need root privileges
	if (server_port < 1024) {
		// check if the user has root privileges
		if (geteuid() != 0) {
			print_log(ERROR,
				  "Invalid port number. To use ports lower "
				  "than 1024 root privileges are needed. "
				  "Quitting.");

			return -1;
		}
	}

	// set default file transfer mode
	strcpy(transfer_mode, "octet");

	// start main loop
	main_loop();

	return 0;
}
