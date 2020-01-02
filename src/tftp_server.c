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
	check_errno(sockfd, "Error while creating listener UDP Socket");

	// zero out server address struct
	memset(&serv_addr, 0, sizeof(serv_addr));

	// fill server address struct: use IPv4 address family
	serv_addr.sin_family = AF_INET;

	// bind to all local interfaces
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// set network port: port numbers below 1024 are privileged ports
	serv_addr.sin_port = htons(port);

	// associate the socket with its local address
	int bound =
	    bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	// check if the socket bound succeeded
	check_errno(bound, "Error while binding listener UDP Socket");

	// server socket successfully started, print connection parameters
	print_log(INFO, "TFTP Server successfully started.");

	// retrieve and print server formatted IP address xxx.xxx.xxx.xxx
	char ip[17];
	inet_ntop(serv_addr.sin_family, (void *)&serv_addr.sin_addr, ip,
		  sizeof(ip));

	// prepare and print log message string
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

void listen_for_packets()
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
	char file_name[512];

	// received transfer mode
	char mode[10];

	// received opcode
	uint16_t opcode;

	// process fork id
	pid_t fork_id;

	// infinite loop
	while (1) {
		// print info log message
		print_log(INFO, "Listening for incoming packets.");

		// recieve the data
		recv_len =
		    recvfrom(listener, (char *)buffer, BUFSIZE, MSG_WAITALL,
			     &cli_addr, (socklen_t *) &cli_size);

		// check for errors
		check_errno(recv_len, "Error while listening for packets");

		// retrieve opcode
		memcpy(&opcode, (uint16_t *) & buffer, 2);
		opcode = ntohs(opcode);

		// retrieve file name
		strcpy(file_name, buffer + 2);

		// retrieve transfer mode
		strcpy(mode, buffer + 3 + strlen(file_name));

		// log info of the received message
		sprintf(log_message,
			"Received opcode: %d, file name: %s and mode: %s.",
			opcode, file_name, mode);
		print_log(INFO, log_message);

		// the only valid opcode at this point is 1 (RRQ)
		if (opcode != 1)
		{
			// print a warning error message
			sprintf(log_message, "Received invalid opcode: %d.",
				opcode);
			print_log(ERROR, log_message);

			// handle invalid opcode received
			handle_invalid_opcode(cli_addr);
		}

		// requested file full path
		char *path = malloc(strlen(base_dir) + strlen(file_name) + 1);

		// setup requested file full path
		strcpy(path, base_dir);
		strcat(path, "/");
		strcat(path, file_name);

		// check if the file actually exists
		if (access(path, F_OK) != -1)
		{
			// file exists, nothing to do
		}
		else
		{
			// file doesn't exist, print an error log message
			sprintf(log_message,
				"Unable to open the requested file: %s.", path);
			print_log(ERROR, log_message);

			// send error message to the client
			handle_file_not_found(listener, cli_addr);

			// loop again
			continue;
		}

		// create a new process by duplicating the calling process
		fork_id = fork();

		// on success, the PID of the child process is returned in the parent,
		// and 0 is returned in the child.  On failure, -1 is returned in the
		// parent, no child process is created, and errno is set appropriately
		if (fork_id == 0)	// child process
		{
			handle_transfer(mode, cli_addr, file_name);
		}
		else if (fork_id > 0)	// parent process
		{
			// just clear the buffer before looping again
			memset(buffer, 0, BUFSIZE);
		}
		else if (fork_id < 0)	// fork() error
		{
			// print a warning error log message
			print_log(ERROR,
				  "Error while creating child process for "
				  "client. Quitting.");

			// exit wirh error
			exit(-1);
		}
	}
}

void handle_transfer(char mode[10], struct sockaddr cli_addr,
		     char file_name[512])
{
	// requested file full path
	char *path = malloc(strlen(base_dir) + strlen(file_name) + 1);

	// set requested file full path
	strcpy(path, base_dir);
	strcat(path, "/");
	strcat(path, file_name);

	// incoming message buffer
	char buffer[BUFSIZE];

	// clear the buffer
	memset(buffer, 0, BUFSIZE);

	// new socket to be used to send data packets
	int data_sock = socket(AF_INET, SOCK_DGRAM, 0);

	// check if the socket was correctly created
	check_errno(data_sock, "Error while creating child process socket");

	// source file pointer
	FILE *src_file;

	// check requested transfer mode
	if (strncmp(mode, "netascii", 8) == 0)	// TEXT MODE
	{
		// print an info log message
		child_log(INFO, "Starting File Transfer in TEXT mode.");

		// open file as text file
		src_file = fopen(path, "r");

		// check if the file was correctly opened
		if (src_file == NULL)
		{
			// if not, print a warning log message
			child_log(ERROR,
				  "Error while opening transfer file. Transfer Cancelled.");

			// send error message to the client
			handle_file_not_found(data_sock, cli_addr);

			// loop again
			return;
		}
		else
		{
			text_mode_transfer(src_file, data_sock, cli_addr);
		}
	}
	else if (strncmp(mode, "octet", 5) == 0)	// BINARY MODE
	{
		// print an info log message
		child_log(INFO, "Starting File Transfer in BINARY mode.");

		// open file as non-text file
		src_file = fopen(path, "rb");

		// check if the file was correctly opened
		if (src_file == NULL) {
			// if not, print a warning log message
			child_log(ERROR,
				  "Error while opening transfer file. Transfer Cancelled.");

			// send error message to the client
			handle_file_not_found(data_sock, cli_addr);

			// loop again
			return;
		}
		else
		{
			binary_mode_transfer(src_file, data_sock, cli_addr);
		}
	}

	// file transfer completed, clear transfer buffer
	memset(buffer, 0, BUFSIZE);

	// close source file
	fclose(src_file);

	// shutdown transfer socket
	shutdown(data_sock, SHUT_RDWR);

	// close transfer socket
	close(data_sock);

	// notify file transfer completed with log message
	sprintf(log_message,
		"File %s correctly transferred to the Client.", file_name);
	child_log(INFO, log_message);

	// kill child process
	exit(0);
}

void text_mode_transfer(FILE * src_file, int data_sock,
			struct sockaddr cli_addr)
{
	// incoming message buffer
	char buffer[BUFSIZE];

	// message opcode
	uint16_t opcode;

	// received message length
	int recv_len;

	// client address length
	size_t cli_size = sizeof(cli_addr);

	// single char store buffer
	char c;

	// chars counter: the first 4 bytes of the transfer buffer
	// must be left empty for the opcode and block number
	int i = 4;

	// data transfer blocks counter
	uint16_t block_counter = 1;

	// until the EOF is found in the opened file
	do {
		// retrieve next char from the source file
		c = fgetc(src_file);

		// avoid adding the EOF to the transfer buffer
		if (c != EOF)
		{
			// copy retrieved char in the transfer buffer
			buffer[i] = c;

			// increase chars counter
			i++;
		}

		// check if either the maximum number of chars or the
		// EOF has been reached
		if (i == MAX + 4 || c == EOF)
		{
			// if so, send the data message to the client
			// opcode = 3 (= DATA)
			opcode = htons(3);

			// prepare block number
			uint16_t block = htons(block_counter);

			// copy opcode to the transfer buffer
			memcpy(buffer, &opcode, 2);

			// copy block_number to the transfer buffer
			memcpy(buffer + 2, &block, 2);

			// send transfer buffer to the client
			recv_len =
			    sendto(data_sock,
				   buffer,
				   i,
				   MSG_CONFIRM,
				   (const struct sockaddr *) &cli_addr,
				   sizeof(cli_addr));

			// check for errors
			check_errno(recv_len,
				    "Error while sending data packet in text mode");

			// if debugging is enabled
			if (DEBUG)
			{
				// print debugging info log message
				sprintf
				    (log_message,
				     "Data packet with block number %d sent. Waiting "
				     "to receive ACK response.", block_counter);
				child_log(INFO, log_message);
			}

			// clear transfer buffer
			memset(buffer, 0, BUFSIZE);

			// wait for ACK response from the client
			recv_len = recvfrom(data_sock, (char *)
					    buffer,
					    BUFSIZE,
					    MSG_WAITALL, (struct sockaddr *)
					    &cli_addr,
					    (socklen_t *) & cli_size);

			// check for errors
			check_errno(recv_len,
				    "Error while receiving ACK packet.");

			// retrieve block number from transfer buffer
			memcpy(&block, (uint16_t *) & buffer[2], 2);

			// deserialize block number
			block = ntohs(block);

			// if debugging is enabled
			if (DEBUG)
			{
				// print debugging info log message
				sprintf
				    (log_message,
				     "ACK response received for block number: %d.",
				     block);
				child_log(INFO, log_message);
			}

			// before sending next data message, check ack message block number
			if (block != block_counter)
			{
				// print a warning error log
				child_log(ERROR,
					  "Sent block number and ACK packet block number do not "
					  "match. Transfer cancelled.");

				// cancel tranfer, close source file
				fclose(src_file);

				// shutdown transfer socket
				shutdown(data_sock, SHUT_RDWR);

				// close transfer socket
				close(data_sock);

				// exit with error
				exit(-1);
			}

			// reset chars counter for new transfer
			i = 4;

			// increase block number counter
			block_counter++;
		}
	}
	while (c != EOF);
}

void binary_mode_transfer(FILE * src_file, int data_sock,
			  struct sockaddr cli_addr)
{
	// incoming message buffer
	char buffer[BUFSIZE];

	// message opcode
	uint16_t opcode;

	// received message length
	int recv_len;

	// client address length
	size_t cli_size = sizeof(cli_addr);

	// size in bytes of the data read from the file
	size_t dim = 0;

	// chars counter: the first 4 bytes of the transfer buffer
	// must be left empty for the opcode and block number
	int i = 4;

	// data transfer blocks counter
	uint16_t block_counter = 1;

	// until the EOF is found in the opened file
	do {
		// retrieve next char from the file
		dim = fread(&buffer[i], 1, 1, src_file);

		// increase chars counter
		i += dim;

		// check if either the maximum number of chars or the
		// EOF has been reached
		if (i == MAX + 4 || dim != 1)
		{
			// if so, send the message to the client
			// opcode = 3 (= DATA)
			opcode = htons(3);

			// set block number
			uint16_t block = htons(block_counter);

			// copy opcode to the transfer buffer
			memcpy(buffer, &opcode, 2);

			// copy block_number to the transfer buffer
			memcpy(buffer + 2, &block, 2);

			// send transfer buffer to the client
			recv_len =
			    sendto(data_sock,
				   buffer, i,
				   MSG_CONFIRM, (const struct sockaddr *)
				   &cli_addr, sizeof(cli_addr));

			// check for errors
			check_errno(recv_len,
				    "Error while sending data packet in binary mode");

			// if debugging is enabled
			if (DEBUG)
			{
				// print info log message
				sprintf
				    (log_message,
				     "Data packet with block number %d sent. Waiting "
				     "to receive ACK response.", block_counter);
				child_log(INFO, log_message);
			}

			// clear transfer buffer
			memset(buffer, 0, BUFSIZE);

			// wait for ACK response from the client
			recv_len = recvfrom(data_sock, (char *)
					    buffer,
					    BUFSIZE,
					    MSG_WAITALL, (struct sockaddr *)
					    &cli_addr,
					    (socklen_t *) & cli_size);

			// check for errors
			check_errno(recv_len,
				    "Error while receiving ACK packet.");

			// retrieve block number from transfer buffer
			memcpy(&block, (uint16_t *) & buffer[2], 2);

			// deserialize block number
			block = ntohs(block);

			// if debugging is enabled
			if (DEBUG)
			{
				// print info log message
				sprintf
				    (log_message,
				     "ACK response received for block number: %d.",
				     block);
				child_log(INFO, log_message);
			}

			// before sending next data packet, check ack message block number
			if (block != block_counter)
			{
				// print a warning error log
				child_log(ERROR,
					  "Sent block number and ACK packet block number do not "
					  "match. Transfer cancelled.");

				// cancel tranfer, close source file
				fclose(src_file);

				// shutdown transfer socket
				shutdown(data_sock, SHUT_RDWR);

				// close transfer socket
				close(data_sock);

				// exit with error
				exit(-1);
			}

			// reset chars counter for new transfer
			i = 4;

			// increase block number counter
			block_counter++;
		}
	}
	while (dim == 1);

}

void handle_invalid_opcode(struct sockaddr cli_addr)
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
	int sent_len = sendto(listener,
			      buffer,
			      strlen(error_message) + 4,
			      MSG_CONFIRM,
			      (const struct sockaddr *)&cli_addr,
			      sizeof(cli_addr));

	// check for errors
	check_errno(sent_len, "Error while sending invalid opcode error message");

	// error message correctly sent
	print_log(INFO, "Error message correctly sent.");
}

void handle_file_not_found(int socket, struct sockaddr cli_addr)
{
	// terminating end string
	uint8_t end_string = 0;

	// response message buffer
	char buffer[BUFSIZE];

	// response message error message text
	char error_message[512];

	// set opcode (ERROR = 5)
	uint16_t opcode = htons(5);

	// set error code (1 = File not found)
	uint16_t error_code = htons(1);

	// copy opcode to the transfer buffer
	memcpy(buffer, &opcode, 2);

	// copy error code to the transfer buffer
	memcpy(buffer + 2, &error_code, 2);

	// set error message text
	strcpy(error_message, "File not found");

	// copy error message to the tranfer buffer
	strcpy(buffer + 2, error_message);

	// add final terminating end string to the buffer
	memcpy(buffer + strlen(error_message) + 2, &end_string, 1);

	// send error message to the TFTP client
	int sent_len = sendto(socket,
			      buffer,
			      strlen(error_message) + 4,
			      MSG_CONFIRM,
			      (const struct sockaddr *)&cli_addr,
			      sizeof(cli_addr));

	// check for errors
	check_errno(sent_len, "Error while sending file not found error message");

	// error message correctly sent
	print_log(INFO, "Error message correctly sent.");
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
	// check if the port and base directory arguments were provided
	if (argc != 3) {
		print_log(ERROR,
			  "Invalid number of arguments. "
			  "Usage: tftp_server <port> <base directory>. "
			  "Quitting.");

		return -1;
	}

	// set listener server port
	int port = atoi(argv[1]);

	// check if the given port need root privileges
	if (port < 1024) {
		// check if the user has root privileges
		if (geteuid() != 0) {
			print_log(ERROR,
				  "Invalid port number. To use ports lower "
				  "than 1024 root privileges are needed. "
				  "Quitting.");

			return -1;
		}
	}

	// check if the given directory path is valid
	DIR *dir = opendir(argv[2]);
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
		print_log(ERROR,
			  "The provided directory path is not valid. Please "
			  "provide the path to an existing directory. "
			  "Quitting.");

		// return with errors
		return -1;
	}
	else
	{
		// opendir() failed for other unknown reasons
		print_log(ERROR,
			  "An unexpected error happened while opening the "
			  "provided directory. Quitting.");

		return -1;
	}

	// create listener UDP server
	listener = createUDPSocket(port);

	// check if the listener socket was correctly created
	check_errno(listener, "Error while creating listener socket. Quitting.");

	// start main loop
	listen_for_packets();

	// return with no errors
	return 0;
}
