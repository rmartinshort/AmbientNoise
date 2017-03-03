/* header file for all_ant_prep

global constants:
NPTS_SAC_FILE_MAX
SECS_PER_DAY


subroutines needing declarations:
read_sac_d
write_sac_d
initialize_spectrum
compute_mean_from_array
find_number_of_blanks
fill_gap
append_array
find_and_remove_excess_samples
prep_signal
fftw_forward
generate_instrument_response
decon_response_function
flt4
fftw_backward
decimate_signal
filter_and_smooth
whiten_rp

*/

/* standard headers */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
//#include <complex.h>
#include <fftw3.h>
//#include <complex.h>
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

/* user headers */
#include "mysac.h"
#include "evresp.h"

/* global constants */
#define NPTS_SAC_FILE_MAX 17280000
#define SECS_PER_DAY 86400
#define PI 3.14159265358979311600
#define N_MAX_ZEROES 50
#define N_MAX_POLES 50
#define PI 3.14159265358979311600


/* structures */
typedef struct {
        int log_N;
        int N;
        double *re;
        double *im;
} SPECTRUM;

typedef struct {
	double real;
	double imag;
} COMPLEX_RP;

typedef struct {
	int n_zeroes;
	int n_poles;
	double scale;
	COMPLEX_RP poles[N_MAX_POLES];
	COMPLEX_RP zeroes[N_MAX_ZEROES];
} POLE_ZERO;


//additions to use pole zero files
void print_sac_pole_zero_file(POLE_ZERO *pz);
void zero_pole_zero(POLE_ZERO *pz);
int read_sac_pole_zero_file(char *filename, POLE_ZERO *pz_out);
COMPLEX_RP generate_response(POLE_ZERO *pz, double freq);
void generate_pz_response_spectrum(SPECTRUM *spect_out, char *pzFileName, double dw);



/* function declarations */
extern SAC_HD *read_sac_d (char *, double *, SAC_HD *, int );
extern void write_sac_d(char *, double *, SAC_HD *);
void initialize_spectrum(SPECTRUM *spect, int npts);
double compute_mean_from_array(double *arr_in, int npts);
int find_number_of_blanks(SAC_HD *sac_header_1, SAC_HD *sac_header_2);
void fill_gap(double *arr_out, double start, double end, double mean, int n);
void append_array(double *arr_1, int npts_1, double *arr_2, int npts_2);
void find_and_remove_excess_samples(double *signal, double *arr_out, SAC_HD *sac_header);
void prep_signal(double *arr_in, double *arr_out, int npts);
void cos_taper(double *arr_in, double *arr_out, int npts);
void remove_trend(double *arr_in, double *arr_out, int npts);
void remove_mean(double *arr_in, double *arr_out, int npts);
void fftw_forward(double *input, SPECTRUM *spect, int npts);
//void generate_instrument_response(SPECTRUM *response_spect, char *response_dir, char *station, char *component, char *locid, char *net_code, char *year, double dt, int npts);
void decon_response_function(SPECTRUM *data_spect, SPECTRUM *response_spect);
void flt4(SPECTRUM *spect, SPECTRUM *spect_out, double delta_omega, int npow, double f1, double f2, double f3, double f4);
void fftw_backward(SPECTRUM *spectrum, double *arr_out);
void decimate_signal(double *input_signal, double *output_signal, int npts, double dt_in, double dt_out);
void filter_and_smooth(double *arr_in, double *arr_out, int npts, int order, char *filter_type, double fmin, double fmax, double delta, int npasses, int width);
void reject_band(SPECTRUM *spect, SPECTRUM *spect_out, double delta_omega, int npow, double f1, double f2, double f3, double f4);
void smooth_cub(SPECTRUM *spect, SPECTRUM *spect_out, double delta_omega, int smooth_flag, double f1, double f2, double f3, double f4);
void whiten_rp(SPECTRUM *spect, SPECTRUM *spect_out, int npts, int npow, double dt, double f1, double f2, double f3, double f4);
void smooth_1d_array(double *arr_in, double *arr_out, int npts, int n_smooth);
COMPLEX_RP complex_subtract(COMPLEX_RP *comp1, COMPLEX_RP *comp2);
COMPLEX_RP complex_divide(COMPLEX_RP *comp1, COMPLEX_RP *comp2);
COMPLEX_RP complex_multiply(COMPLEX_RP *comp1, COMPLEX_RP *comp2);
COMPLEX_RP conjugate(COMPLEX_RP *comp);
COMPLEX_RP complex_add(COMPLEX_RP *comp1, COMPLEX_RP *comp2);
void free_spectrum(SPECTRUM *spect);
void interpolate_spectrum(SPECTRUM *spect, SPECTRUM *spect_out, double *freqs, int nfreqs_out);
double log_n(double x, double b);
int find_first_sample(SAC_HD *sac_header, int start_jday);
void cut_record(double *arr_in,double *arr_out, SAC_HD *header, int npts_out);
void phase_shift(double t_shift, double dt,int np, SPECTRUM *spect_in);
COMPLEX_RP complex_exp(COMPLEX_RP *comp);
void normalize_record_length(double *arr_in, double *arr_out, SAC_HD *header, int npts_out, int jday);
void append_array_debug(double *arr_1, int npts_1, double *arr_2, int npts_2);


