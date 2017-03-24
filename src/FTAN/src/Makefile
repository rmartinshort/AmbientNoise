#
INST_DIR = ../bin
#BIN  = aftani_c_pgl_AMP
#BIN  = aftani_c_pgl_S
BIN  = ftan_2

fflags = -O2 -Wall -ffixed-line-length-none
cflags = -O2 -Wall -lm -lfftw3

#LDLIBS =   -L/data/isis/rwporrit/local/lib -lfftw3
LDLIBS =   -L/usr/local/lib64 -L/data/dna/software/FFTW/lib

FFLAGS = $(DBG) $(fflags)
CFLAGS = $(DBG) $(cflags)

FC = gfortran
CC = gcc 

DBG =

all :
	$(CC) $(CFLAGS) -c driver_rp.c
	$(CC) $(CFLAGS) -c swapn.c 
	$(CC) $(CFLAGS) -c misc.c
	$(FC) $(FFLAGS) -c aftanpg.f aftanipg.f ftfilt.f fmax.f taper.f trigger.f tapers.f tgauss.f mspline.f pred_cur.f phtovel.f


FOBJS = driver_rms.o swapn.o aftanpg.o aftanipg.o ftfilt.o fmax.o taper.o \
	trigger.o tapers.o tgauss.o mspline.o pred_cur.o misc.o phtovel.o

$(BIN) : $(FOBJS)
	$(FC) $(FFLAGS) $(CFLAGS) $(FOBJS) -o $(INST_DIR)/$(BIN) $(LDLIBS) 

install :: $(BIN)
	install -s $(BIN) $(INST_DIR)

clean ::
	rm -f $(BIN) core.* $(FOBJS)
