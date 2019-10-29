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

