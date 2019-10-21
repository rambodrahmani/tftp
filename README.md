# TFTP
A simple TFTP Server and Client Implementation in C.

# Compilation
The Project comes with a Makefile which can be used to compile everything:
```
$ make
Compiled src/tftp_server.c successfully.
Compiled src/tftp_client.c successfully.
Linking obj/tftp_server.o completed.
Linking obj/tftp_client.o completed.
```
No clean up is made after compilation. You can manually do it using:
```
make cleanup
```

### Project structure
```
TFTP |--src/        Contains source code files (.c)
     |--include/    Contains header files (.h)
     |--obj/        Contains object files (.o) after compilation
     |--bin/        Contains executable files after linking
     |--Makefile    Project Makefile
```

# References
[RFC 1350 -  THE TFTP PROTOCOL (REVISION 2)](https://tools.ietf.org/html/rfc1350)

