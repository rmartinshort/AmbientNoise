#
INST_DIR = ../bin
BIN  = get_start_time.exe

HEADERFLAGS = -I../include
CFLAGS = -Wall -g -O3 -lm

CC = gcc

OBJS = get_start_time.c  ../lib/read_sac.c

all:
	$(CC) $(OBJS) $(CFLAGS) $(HEADERFLAGS) -o $(INST_DIR)/$(BIN)

clean ::
	rm -f $(BIN) core $(FOBJS)


