#RMS Feb 2017

BIN = bin
CFLAGS = -Wall -g -O3
HEADERFLAGS = -I. -I/data/dna/software/FFTW/include
LIBFLAGS = -lfftw3 -lm 
LIBDIRFLAGS = -L/usr/local/lib64 -L/usr/local/lib -L../../lib -L/data/dna/software/FFTW/lib

CC = gcc

all:
	$(CC) pack_spectrums.c $(CFLAGS) $(HEADERFLAGS) $(LIBFLAGS) $(LIBDIRFLAGS) -o $(BIN)/Pack_Spectrums.exe
	$(CC) cor_rp_single_spect.c $(CFLAGS) $(HEADERFLAGS) $(LIBFLAGS) $(LIBDIRFLAGS) -o $(BIN)/Cor_Single_Spect.exe
	$(CC) find_dead_sac_traces.c $(CFLAGS) $(HEADERFLAGS) $(LIBFLAGS) $(LIBDIRFLAGS) -o $(BIN)/Find_Dead_traces.exe
