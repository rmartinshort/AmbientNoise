## RMS Feb 2017
## Compile C code for Hermann code prep

BIN  = bin
CFLAGS = -Wall -g -O3 -lm
CC = gcc
OBJS1 = dispersion_db_to_hermann_input.c
OBJS2 = dispersion_db_to_hermann_input_phonly.c
OBJS3 = dispersion_db_to_hermann_input_phonlyv1.c
 
all:
	$(CC) $(OBJS1) $(CFLAGS) -o $(BIN)/dispersion_db_to_hermann.exe
	$(CC) $(OBJS3) $(CFLAGS) -o $(BIN)/dispersion_db_to_hermann_phv.exe

#$(CC) $(OBJS3) $(CFLAGS) -o $(BIN)/dispersion_db_to_hermann_phv1.exe


clean:
	rm -f $(BIN)/*


