# TFTP
A simple TFTP Server and Client Implementation in C. Only `RRQ`, `DATA`, `ACK`,
and `ERROR` packets have been implemented. The `WRQ` packet is to be
implemented.

# Compilation
The Project comes with a Makefile which can be used to compile everything:
```
$ make
Compiled src/common.c successfully.
Compiled src/tftp_server.c successfully.
Compiled src/tftp_client.c successfully.
Linking obj/tftp_server.o obj/common.o completed.
Linking obj/tftp_client.o obj/common.o completed.
```
No clean up is made after compilation. You can manually do it using:
```
make cleanup
```

### Project structure
```
TFTP |--base_dir/      Contains sample files for testing purposes
     |--bin/           Contains executable files after linking
     |--include/       Contains header files (.h)
     |--obj/           Contains object files (.o) after compilation
     |--references/    Contains reference PDF files
     |--src/           Contains source code files (.c)
     |--Makefile       Project Makefile
```

# TFTP Protocol
Follows a brief introduction to the TFTP Protocol as introduced by
[RFC 1350 -  THE TFTP PROTOCOL (REVISION 2)](https://tools.ietf.org/html/rfc1350)
. TFTP is a very simple protocol used to transfer files. It is from this that
its name comes, Trivial File Transfer Protocol or TFTP.

The protocol was originally designed by Noel Chiappa, and wasredesigned by him,
Bob Baldwin and Dave Clark, with comments fromSteve Szymanski.

It has been implementedon top of the Internet User Datagram protocol (UDP or
Datagram) so it may be used to move files between machines on different networks
implementing UDP. It is designed to be small and easy to implement. Therefore,
it lacks most of the features of a regular FTP. The only thing it can do is read
and write files (or mail) from/to a remote server. It cannot list directories,
and currently has no provisions for user authentication. In common with other
Internet protocols, it passes 8 bit bytes of data.

Three modes of transfer are currently supported: netascii (This is ascii as
defined in "USA Standard Code for Information Interchange" with the
modifications specified in "Telnet Protocol Specification"). Note that it is 8
bit ascii. The term "netascii" will be used throughout this document to mean
this particular version of ascii.); octet (This replaces the "binary" mode of
previous versions of this document.) raw 8 bit bytes; mail,netascii characters
sent to a user rather than a file. (The mail mode is obsolete and should not be
implemented or used.)  Additional modes can be defined by pairs of cooperating
hosts.

Any transfer begins with a request to read or write a file, which also serves to
request a connection. If the server grants the request, the connection is opened
and the file is sent in fixed length blocks of 512 bytes. Each data packet
contains one block of data, and must be acknowledged by an acknowledgment packet
before the next packet can be sent. A data packet of less than 512 bytessignals
termination of a transfer.

Most errors cause termination of the connection. An error is signalled by
sending an error packet. This packet is not acknowledged, and not retransmitted
(i.e., a TFTP server or user mayterminate after sending an error message), so
the other end of the connection may not get it. Therefore timeouts are used to
detect such a termination when the error packet has been lost. Errors are caused
by three types of events: not being able to satisfy the request (e.g., file not
found, access violation, or no such user), receiving a packet which cannot be
explained by a delay or duplication in the network (e.g., an incorrectly formed
packet), and losing access to a necessary resource (e.g., disk full or access
denied during a transfer).

### Initial Connection Protocol
A transfer is established by sending a request (WRQ to write onto a foreign file
system, or RRQ to read from it), and receiving apositive reply, an
acknowledgment packet for write, or the first datapacket for read. In general an
acknowledgment packet will contain the block number of the data packet being
acknowledged. Each data packet has associated with it a block number; block
numbers are consecutive and begin with one. Since the positive response to a
write request is an acknowledgment packet, in this special case the block number
will be zero. (Normally, since an acknowledgment packetis acknowledging a data
packet, the acknowledgment packet will contain the block number of the data
packet being acknowledged). If the reply is an error packet, then the request
has been denied.

### TFTP Packets
TFTP supports five types of packets, all of which have been mentioned above:
```
    opcode  operation
      1     Read request (RRQ)
      2     Write request (WRQ)
      3     Data (DATA)
      4     Acknowledgment (ACK)
      5     Error (ERROR)
```
The TFTP header of a packet contains the opcode associated with that packet.

RRQ and WRQ packets (opcodes 1 and 2 respectively) have the formats hown below:
```
 2 bytes    string     1 byte    string    1 byte
+--------+------------+------+------------+------+
| Opcode |  Filename  |   0  |    Mode    |   0  |
+--------+------------+------+------------+------+
```
The file name is a sequence of bytes in netascii terminated by a zero byte. The
mode field contains the string "netascii", "octet", or "mail" (or any
combination of upperand lower case, such as "NETASCII", "NetAscii", etc.) in
netascii indicating the three modes defined in the protocol.

