#
INST_DIR = bin
BIN  = itomo.exe

fflags = -O2 -ffixed-line-length-none

FFLAGS = $(DBG)  $(fflags) $(shlib)

FC = gfortran

FOBJS = tomo_sp_cu.o help.o init.o menu.o set_defaults.o \
	reader.o m1.o rejector.o rejedel.o perc_map.o swt_sph.o \
	line.o tria.o trass.o model.o spl.o \
	res_anal.o res_form.o res_tria.o res_outp.o

$(BIN) : $(FOBJS)
	$(FC) $(FFLAGS) $(CFLAGS) $(FOBJS) -o $(INST_DIR)/$(BIN) $(LDLIBS)

clean ::
	rm -f $(BIN) core $(FOBJS)
