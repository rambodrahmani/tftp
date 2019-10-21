#-------------------------------------------------------------------------------
# File: Makefile
#       Project Makefile.
#
# Author: Rambod Rahmani <rambodrahmani@autistici.org>
#         Created on 21/10/2019.
#-------------------------------------------------------------------------------

# set compiler
CC = gcc

# set compilation flags
CFLAGS   = -Wall

# set linker
LINKER = gcc

# set linking flags
LFLAGS = -Wall

# header files directory
INCDIR = include

# source files directory
SRCDIR = src

# object files directory
OBJDIR = obj

# executable files directory
BINDIR = bin

# define rm as rm --force
rm  = rm -f

# all targets
all: $(OBJDIR)/tftp_server.o $(OBJDIR)/tftp_client.o $(BINDIR)/tftp_server $(BINDIR)/tftp_client

# compile TFTP Server source files
$(OBJDIR)/tftp_server.o: $(SRCDIR)/tftp_server.c $(INCDIR)/tftp_server.h
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully."

# compile TFTP Client source files
$(OBJDIR)/tftp_client.o: $(SRCDIR)/tftp_client.c $(INCDIR)/tftp_client.h
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully."

# link TFTP Server object files
$(BINDIR)/tftp_server: $(OBJDIR)/tftp_server.o
	@$(LINKER) $< $(LFLAGS) -o $@
	@echo "Linking "$<" completed."

# link TFTP Client object files
$(BINDIR)/tftp_client: $(OBJDIR)/tftp_client.o
	@$(LINKER) $< $(LFLAGS) -o $@
	@echo "Linking "$<" completed."

# clean up utility
clean:
	@$(rm) $(OBJDIR)/tftp_server.o $(OBJDIR)/tftp_client.o
	@$(rm) $(BINDIR)/tftp_server $(BINDIR)/tftp_client
	@echo "Cleanup completed."

