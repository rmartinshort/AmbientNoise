## RMS April 2017
## Compile checkerboard test for the ambient noise tomo

BIN  = bin
CFLAGS = -Wall -g -lm
CC = gcc

FOBJS = create_ant_checkerboard.c libraytracerrp.c 

all ::
	$(CC) $(CFLAGS) $(FOBJS) -o $(BIN)/create_ant_checkerboard.exe

clean ::
	rm -f *.o $(BIN)/*
