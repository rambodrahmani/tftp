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

all: $(OBJDIR)/tftp_server.o $(OBJDIR)/tftp_client.o $(BINDIR)/tftp_server $(BINDIR)/tftp_client

$(OBJDIR)/tftp_server.o: $(SRCDIR)/tftp_server.c $(INCDIR)/tftp_server.h
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully."

$(OBJDIR)/tftp_client.o: $(SRCDIR)/tftp_client.c $(INCDIR)/tftp_client.h
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully."

$(BINDIR)/tftp_server: $(OBJDIR)/tftp_server.o
	@$(LINKER) $< $(LFLAGS) -o $@
	@echo "Linking "$<" completed."

$(BINDIR)/tftp_client: $(OBJDIR)/tftp_client.o
	@$(LINKER) $< $(LFLAGS) -o $@
	@echo "Linking "$<" completed."

clean:
	@$(rm) $(OBJDIR)/tftp_server.o $(OBJDIR)/tftp_client.o $(BINDIR)/tftp_server $(BINDIR)/tftp_client
	@echo "Cleanup completed."

