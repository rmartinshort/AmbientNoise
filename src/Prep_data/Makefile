## RMS Feb 2017
## Compile C code for preparing data for ANT


BIN  = bin
CFLAGS = -Wall -g -O3 -lm
HEADERFLAGS = -I../../include -I/data/dna/software/FFTW/include
LIBDIRFLAGS = -L/usr/local/lib64 -L/usr/local/lib -L../../lib -L/data/dna/software/FFTW/lib
LIBFLAGS = -lfftw3 -levresp
CC = gcc
OBJS = all_ant_prep_128s_5_100.c lib_prep.c
 
all:

	$(CC) $(OBJS) $(CFLAGS) $(HEADERFLAGS) $(LIBDIRFLAGS) $(LIBFLAGS) -o $(BIN)/Prep_ANT_data_128_5_100.exe
#	$(CC) $(CFLAGS) dispersion_db_to_hermmann_input.c -o $(BIN)/Dispersion_dp_2_hermann.exe

clean:
	rm -f $(BIN) core $(FOBJS)


