#
#
#RMS - warning - this makefile is a little 'unconventional' - it works to compile these noise
#directvity codes, but should eventually be put in the same format as the other makefiles
#
#
#
INST_DIR = bin 
BIN  = locate_energy
BIN2 = get_nth_percentile_from_locate_energy
BIN3 = compute_slowness_vector
BIN4 = stack_normalized_energy
LIB =  ../lib

cflags = -Wall -g -lm

CFLAGS = $(DBG) $(cflags)

CC = gcc

FOBJS = locate_energy.c lib_prgm_rob.c $(LIB)/read_sac.c $(LIB)/write_sac.c
COBJS = get_nth_percentile_from_locate_energy.c
EOBJS = compute_slowness_vector.c
DOBJS = stack_normalized_energy.c


$(BIN) : $(FOBJS) $(COBJS) $(EOBJS) $(DOBJS)
	$(CC) $(CFLAGS) $(FOBJS) -o $(BIN)
	$(CC) $(CFLAGS) $(COBJS) -o $(BIN2)
	$(CC) $(CFLAGS) $(EOBJS) -o $(BIN3)
	$(CC) $(CFLAGS) $(DOBJS) -o $(BIN4)

install :: $(BIN) $(BIN2) $(BIN3) $(BIN4)
	install -s $(BIN) $(INST_DIR)
	install -s $(BIN2) $(INST_DIR)
	install -s $(BIN3) $(INST_DIR)
	install -s $(BIN4) $(INST_DIR)

clean ::
	rm -f $(BIN) $(BIN2) $(BIN3) $(BIN4) core $(FOBJS)
