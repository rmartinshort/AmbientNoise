#RMS Feb 2017

BIN = bin
CFLAGS = -Wall -g -O2
HEADERFLAGS = -I../../../include -I/data/dna/software/FFTW/include -I.
LIBFLAGS = -lfftw3 -lm 
LIBDIRFLAGS = -L/usr/local/lib64 -L/usr/local/lib -L../../../../lib -L/data/dna/software/FFTW/lib -L./

CC = gcc

#Note that the phase and group extraction versions of this code are exactly the same, save one change in the 
#read_ftan function in lib_select_dispersion_group.c to read group vels instead of phase vels


all:
	$(CC) select_dispersion_snr_and_dist_phase_only.c lib_select_dispersion_phase.c $(CFLAGS) $(HEADERFLAGS) $(LIBFLAGS) $(LIBDIRFLAGS) -o $(BIN)/Select_Dispersion_SNR_and_DIST_phase_only.exe
	$(CC) select_dispersion_snr_and_dist_group_only.c lib_select_dispersion_group.c $(CFLAGS) $(HEADERFLAGS) $(LIBFLAGS) $(LIBDIRFLAGS) -o $(BIN)/Select_Dispersion_SNR_and_DIST_group_only.exe
