#RMS March 2017
#Compile the generate dispersion codes

BIN = bin
LIBFLAGS = -lm 

CC = gcc
FC = gfortran

all:
	$(FC) -c disper.f -o disper.o
	$(FC) -c mchdepsun.f -o mchdepsun.o
	$(CC) -c lib_mc_inversion_rp.c -o lib_mc_inversion_rp.o

	$(FC) generate_dispersion.c lib_mc_inversion_rp.o disper.o mchdepsun.o $(LIBFLAGS) -o $(BIN)/generate_dispersion.exe

