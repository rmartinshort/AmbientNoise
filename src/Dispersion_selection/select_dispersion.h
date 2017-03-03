/*
 *  select_dispersion.h
 *  
 *
 *  Created by Rob Porritt on 11/8/10.
 *  Copyright 2010 UC Berkeley. All rights reserved.
 *
 */

//Headers
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <fftw3.h>
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif
#include "mysac.h"
#include <float.h>


#define PI 3.14159265358979311600
#define MAX_VEL 5.0
#define MIN_VEL 1.0
#define NAN_VALUE -1  //important to keep the nan value a negative value. The output function checks for and rejects negative values.
#define MAX_PER_OUT 70
#define MAX_COR 1337641
//#define MAX_COR 1235074 
#define SNR_MIN_MID 20 
#define DIST_MIN_MID 3 

//holding all we care about from the ftan measurement
typedef struct {
	unsigned int N;          //number of measurements
	float *per;     //periods of measurements
	float *ph;      //phase velocities corresponding to the periods
} DISPERSION;

//similar to DISPERSION, but remove period pointer because this is for the interpolation
typedef struct {
	unsigned int N;          //number of measurements
	float ph[MAX_PER_OUT];      //phase velocities
} VELOCITIES;

//purely for metadata - can all come from sac headers if properly packed
typedef struct {
	float lat1;     //latitude of station 1
	float lon1;     //longitude of station 1
	float lat2;     //latitude of station 2
	float lon2;     //longitude of station 2
	float dist;     //interstation distance
	float azimuth;  //interstation azimuth
	float ndays;    //sac header user0
	char sta1[6];   //name of station 1 (for comments)
	char sta2[6];   //name of station 2 (for comments)
} METADATA;

//F(w) structure
typedef struct {
	float *re;  //real component
	float *im;  //imaginary component 
	int N;      //number of points in the spectrum
} SPECTRUM;

//Function prototypes
int read_sac(char *, float *, SAC_HD *, int );
int read_ftan(char *fname, DISPERSION *disp);
void filter4(SPECTRUM *spect, SPECTRUM *spect_out, float delta_omega, int npow, float f1, float f2, float f3, float f4);
void fill_metadata(SAC_HD *sac_header, METADATA *meta);
void interpolate_dispersion(DISPERSION *disp, VELOCITIES *vel_out, float permin, float permax, float perinc);
float find_minimum_dispersion(DISPERSION *disp);
float find_maximum_dispersion(DISPERSION *disp);
int compute_nodes(float min, float max, float inc);
void compute_snr(SPECTRUM *spect, VELOCITIES *ftan, float *snr, METADATA *met, int npts, float delta, float min, float max, float permin, float perinc);
void compute_disp_snr(SPECTRUM *spect, DISPERSION *ftan, float *snr, METADATA *met, int npts, float delta, float min, float max, float permin, float permax, float perinc,
					  SPECTRUM *spect_filt, DISPERSION *snrDispersion, 
					  fftw_complex *ff_spect, double *temp, fftw_plan plan_backward);





