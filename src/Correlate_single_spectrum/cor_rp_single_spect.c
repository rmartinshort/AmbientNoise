/*
cor_rp_sd_and_mean_io_block_float_2.c

1. Program reads in a database file and uses that to track the sac files to correlate
2. Then allocates enough memory for a day's worth of data including:
	n_stations spectrums (for 1 day at 1sps its 43201 real and 43201 imaginary = 86402 samples/day)
	n_correlations mean
	n_correlations standard deviation
We will also need some temporary arrays

original allocations:
        sig_1_re = malloc(sizeof(sig_1_re) * (NPTS_SAC_FILE_MAX / 2 + 1));
        sig_1_im = malloc(sizeof(sig_1_im) * (NPTS_SAC_FILE_MAX / 2 + 1));
        sig_2_re = malloc(sizeof(sig_2_re) * (NPTS_SAC_FILE_MAX / 2 + 1));
        sig_2_im = malloc(sizeof(sig_2_im) * (NPTS_SAC_FILE_MAX / 2 + 1));

        sig_out = malloc(sizeof(sig_out) * NPTS_SAC_FILE_MAX);

        cor = malloc(sizeof(cor) * (lag * 2+1));
        mean = malloc(sizeof(mean) * (lag * 2 + 1));
        sd = malloc(sizeof(sd) * (lag * 2 + 1));
        temp_cor = malloc(sizeof(temp_cor) * (lag * 2 + 1));

        initialize_spectrum(&spect_1,NPTS_SAC_FILE_MAX);
        initialize_spectrum(&spect_2,NPTS_SAC_FILE_MAX);
        initialize_spectrum(&spect_temp,NPTS_SAC_FILE_MAX);

The first four are for reading the spectrum files. We can use those in a reading loop, then as soon as a file is read, dump it into the 1 day's worth of data mega-array. They can stick around the whole time and just be cleared at the end.

sig_out is returned after the cross correlation is computed - its the full time domain representation of the spectrally computed cross correlation

cor, mean, and sd are extracted from sig_out and are reformatted to only contain data from -lag to +lag 

temp_cor is used for stacking and may not be necessary in this version

spect_1 is filled with sig_1_re and sig_1_im
spect_2 is filled with sig_2_re and sig_2_im
spect_temp is the spectrally computed cross correlation

3. inside the event (day) loop, read all of the spectrums for that day. If a file exists for the mean and standard deviation correlation, read those in as mean and sd. 

4. Loop over each station pair

5. at the end of a day's worth of data, write everything and then repeat with the next event.


/// 7.19.2010  /////
allocate memory for:
all_spectrums_station_1
  2 * 31 * npts_spectrum (ant_db.npts_spectrum)
all_spectrums_station_2
  2 * 31 * npts_spectrum (ant_db.npts_spectrum)
temp_spect_1
  npts_spectrum (ant_db.npts_spectrum)
temp_spect_2
  npts_spectrum (ant_db.npts_spectrum)
cor_spect
  npts_spectrum (ant_db.npts_spectrum)
ifft
  npts_spectrum * 2
cor
  2 * lag + 1
mean_cor
  2 * lag + 1
sd_cor
  2 * lag + 1


initialization
for each station 1 {
  read spectrums of station 1
  read header of station 1
  for each station 2 {
     read spectrums of station 2
     read header of station 2
     for each event {
       setup current spectrum 1
       setup current spectrum 2
       compute correlation
       ifft
       cut to +/- lag
       compute mean and sd
     }
   update header for correlation
   write mean and sd as sac files
  }
}

Also change usage to use parameter file instead
old usage:  data_base waveform_base_directory output_directory lag_time
parameter file format
database_name
output_directory
lag_time



*/
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
//#include <complex.h>
#include <fftw3.h>
//#include <complex.h>
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif
#include "ant_db.h"
#include "mysac.h"
#include <sys/stat.h>
#include <sys/time.h>

#define NPTS_LH_DAY 86400

#define EARTH_RADIUS 6371
#define PI 3.14159265358979311600
#define NPTS_SAC_FILE_MAX 86400
#define SECS_PER_DAY 86400

typedef struct {
        int log_N;
        float *re;
        float *im;
	int N;
} SPECTRUM;

/*
#define MAX_STATIONS 1000
#define MAX_EVENTS 32
#define MAX_ARRAYS_OUTPUT 10000

typedef struct EVENT {
        int year;
        int month;
        int day;
        int jday;
} EVENT;

typedef struct STATION {
        char name[7];
        char component[4];
        char network[3];
	float latitude;
	float longitude;
	float elevation;
        int on_off[MAX_EVENTS];
} STATION;

typedef struct ANT_DB {
	char base_directory[100];
	int npts_spectrum;
	int npts_time_series;
        int n_events;
        int n_stations;
        STATION station_table[MAX_STATIONS];
        EVENT event_table[MAX_EVENTS];
	SAC_HD standard_header;
} ANT_DB;
*/

/* subroutines needed:
initialize_spectrum
	log_n
read_sac
write_sac
cross_correlate_rp
fftw_backward
km_dist
	degree_dist
recursive_mean
recursive_standard_deviation
compute_event_station_azimuth
read_spectrum_header
read_spectrum_data
get_spectrum_from_data
*/
float degree_dist(float lat1, float long1, float lat2, float long2);
float km_dist(float lat1, float long1, float lat2, float long2);
float recursive_mean(float new_value, float old_mean, int n);
float recursive_standard_deviation(float new_value, float current_mean, float prev_value, int n);
float compute_event_station_azimuth(float lat1, float long1, float lat2, float long2);
void fftw_backward(SPECTRUM *spectrum, float *arr_out);
void initialize_spectrum(SPECTRUM *spect, int npts);
extern SAC_HD *read_sac (char *, float *, SAC_HD *, int );
extern void write_sac (char *, float *, SAC_HD *);
void cross_correlate_rp(SPECTRUM *spect_1, SPECTRUM *spect_2, SPECTRUM *spect_out);
float log_n(float x, float b);
int read_spectrum_header(char *fname, SAC_HD *SHD);
int read_spectrum_data(char *fname, float *spect, int npts);
void get_spectrum_from_data(float *in, SPECTRUM *out, int rec, int npts_rec, int station_num, int n_events);
int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1);
void get_spectrum_from_file(FILE *ff, SPECTRUM *out, int rec, int npts_rec, int station_num, int n_events, float *workingArray);

//read a binary formatted header file. Alters a pointer to the new header as SHD
int read_spectrum_header(char *fname, SAC_HD *SHD) {
        /* local variable used to open the file */
        FILE *fsac;

        /* try to open the file */
        if (! (fsac = fopen(fname,"rb"))) {
                fprintf(stderr,"ERROR: read_spectrum_header: file %s not found!\n",fname);
                return 0;
        }

        /* initialize the header */
        if (! SHD ) SHD = &SAC_HEADER;

        /* read the binary formatted header data and store in SHD */
        fread(SHD,sizeof(SAC_HD),1,fsac);

        /* close up the file */
        fclose(fsac);


        /* return a success */
        return 1;
}
//END

//read the data from the spectrum. Returns as an unformatted array spect
int read_spectrum_data(char *fname, float *spect, int npts) {
        /* local variable used to open the file */
        FILE *fsac;

        /* try to open the file */
        if (! (fsac = fopen(fname,"rb"))) {
                fprintf(stderr,"ERROR: read_spectrum_data: file %s not found!\n",fname);
                return 0;
        }

        /* read the binary formatted data and store in spect */
        fread(spect,sizeof(spect),npts,fsac);

        /* close up the file */
        fclose(fsac);

        /* return a sucess */
        return 1;
}
//END

//similar to below, but when the memory is limited we need to repeated look up from the file 
void get_spectrum_from_file(FILE *ff, SPECTRUM *out, int rec, int npts_rec, int station_num, int n_events, float *workingArray) {
	int i;
	int nvar;
	nvar = rec * npts_rec + station_num * npts_rec * n_events;
	//size of the stored data is float
	//working array is what we read into. it will contain the real and imaginary components. Once loaded, we parse it into the real and imaginary parts of the spectrum
	printf("%ld %d %d %d %d %d\n",sizeof(float), rec, (int)floor(npts_rec/2+0.5), station_num, n_events,nvar);

	fseek(ff, sizeof(float) * nvar, SEEK_SET);
	fread(workingArray, sizeof(float), npts_rec, ff);
	for (i=0; i<floor(npts_rec/2+0.5); i++) {
		out->re[i] = workingArray[i];
		out->im[i] = workingArray[i+(int)floor(npts_rec/2+0.5)];
	}


	return;
}

//decides where to start getting data from the array output by read_spectrum_data for a given record rec where each has npts_rec
void get_spectrum_from_data(float *in, SPECTRUM *out, int rec, int npts_rec, int station_num, int n_events) {
        /* local variable */
        int i, r_i, i_i;

//	printf("%d\n",rec * npts_rec + station_num * npts_rec * 31);

        /* loop over the points. Be careful about where it starts and ends */
        for (i=0; i<floor(npts_rec / 2+0.5); i++) {
                r_i = i + rec * npts_rec + station_num * npts_rec * n_events;
                i_i = r_i + floor( npts_rec / 2 + 0.5);
                out->re[i] = in[r_i];
                out->im[i] = in[i_i];
        }

        /* return to the calling program */
        return;
}
//END

//cross_correlate///log_n
//log of a generic base - with error reporting
float log_n(float x, float b) {
        if (x > 0 && b > 0) {
                return log(x) / log(b);
        } else if (x <= 0) {
                fprintf(stderr,"log_n Error: x must be positive (value given: %f)\n", x);
                return -1.0;
        } else if (b <=0) {
                fprintf(stderr,"log_n Error: b must be positive (value given: %f)\n", b);
        	return -1.0;
	}
        return 0;
}
///END
void cross_correlate_rp(SPECTRUM *spect_1, SPECTRUM *spect_2, SPECTRUM *spect_out) {
	int i;
	spect_out->N = spect_1->N;
	spect_out->log_N = spect_1->log_N;	

	for (i=0; i<spect_1->N; i++) {
		spect_out->re[i] = spect_1->re[i] * spect_2->re[i] + spect_1->im[i] * spect_2->im[i];
		spect_out->im[i] = spect_1->im[i] * spect_2->re[i] - spect_1->re[i] * spect_2->im[i];
	}

	return;
}
////END
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
        SAC_HD *read_sac (char *fname, float *sig, SAC_HD *SHD, int nmax)
/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/
{
 FILE *fsac;
/*..........................................................................*/
        fsac = fopen(fname, "rb");
        if ( !fsac )
        {
         //fclose (fsac);
         return NULL;
        }

        if ( !SHD ) SHD = &SAC_HEADER;

         fread(SHD,sizeof(SAC_HD),1,fsac);

         if ( SHD->npts > nmax )
         {
          fprintf(stderr,
           "ATTENTION !!! in the file %s the number of points is limited to %d\n",fname,nmax);

          SHD->npts = nmax;
         }

         fread(sig,sizeof(float),(int)(SHD->npts),fsac);

        fclose (fsac);

   /*-------------  calcule de t0  ----------------*/
   {
        int eh, em ,i;
        float fes;
        char koo[9];

        for ( i = 0; i < 8; i++ ) koo[i] = SHD->ko[i];
        koo[8] = '\0';

        SHD->o = SHD->b + SHD->nzhour*3600. + SHD->nzmin*60 +
         SHD->nzsec + SHD->nzmsec*.001;

        sscanf(koo,"%d%*[^0123456789]%d%*[^.0123456789]%g",&eh,&em,&fes);

        SHD->o  -= (eh*3600. + em*60. + fes);
   /*-------------------------------------------*/}
        return SHD;
}
//////END

////
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
        void write_sac (char *fname, float *sig, SAC_HD *SHD)
/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/
{
 FILE *fsac;
 int i;
// int *sig2;

/*..........................................................................*/
        fsac = fopen(fname, "wb");

        if ( !SHD ) SHD = &SAC_HEADER;


        SHD->iftype = (long)ITIME;
        SHD->leven = (long)TRUE;

        SHD->lovrok = (long)TRUE;
        SHD->internal4 = 6L;

//		sig2 = malloc(SHD->npts * sizeof(*sig2));


  /*+++++++++++++++++++++++++++++++++++++++++*/
     SHD->depmin = sig[0];
     SHD->depmax = sig[0];

   for ( i = 0; i < SHD->npts ; i++ )
   {
    if ( SHD->depmin > sig[i] ) SHD->depmin = sig[i];
    if ( SHD->depmax < sig[i] ) SHD->depmax = sig[i];
   }

//		for (j=0; j<SHD->npts; j++) {
//			sig2[j] = (int) sig[j];
//		}

         fwrite(SHD,sizeof(SAC_HD),1,fsac);

         fwrite(sig,sizeof(float),(int)(SHD->npts),fsac);

         fclose (fsac);

	return;
}
////END

//initialize_spectrum
void initialize_spectrum(SPECTRUM *spect, int npts) {
	spect->N = npts / 2 + 1;
	spect->log_N = floor(log_n(npts,2)+0.5);
	spect->re = malloc(sizeof(*(spect->re)) * spect->N);
	spect->im = malloc(sizeof(*(spect->im)) * spect->N);
	return;
}

///fftw_backward////
void fftw_backward(SPECTRUM *spectrum, float *array_out)
{
  int i;
  double *temp;
  fftw_complex *spect;
  fftw_plan plan_backward;
  int n;

  spect = fftw_malloc( sizeof ( fftw_complex ) * spectrum->N);

  //initialize spect from spectrum
  n = (spectrum->N - 1) * 2;

  for (i=0; i<spectrum->N; i++) {
        spect[i][0] = spectrum->re[i];
        spect[i][1] = spectrum->im[i];
  }

  temp = fftw_malloc ( sizeof (*temp) * n );
  plan_backward = fftw_plan_dft_c2r_1d(n,spect,temp,FFTW_ESTIMATE);
  
  fftw_execute(plan_backward);
  fftw_destroy_plan(plan_backward);

  fftw_free(spect);
  for ( i = 0; i < n; i++ )
  {
    array_out[i] = temp[i] / (float) n;
  }
  
//  free_spectrum(spectrum); 
  fftw_free ( temp );
	
  return;
}
////END

//compute the distance in degrees
float degree_dist(float lat1, float long1, float lat2, float long2) {
	return acos(sin(lat1 * PI / 180.0) * sin(lat2 * PI / 180.0) + cos(lat1 *PI / 180.0) *cos(lat2 * PI / 180.0) * cos((long2 - long1) * PI / 180.0)) * 180.0 / PI;
}
//END


//compute the distance in kilometers
float km_dist(float lat1, float long1, float lat2, float long2) {
        return degree_dist(lat1,long1,lat2,long2) * PI / 180.0 * EARTH_RADIUS;
}
//END

//compute the event-station azimuth
float compute_event_station_azimuth(float lat1, float long1, float lat2, float long2) {
	float colatB, inter_long;
	colatB = (90.0 - lat2) * PI / 180.0;
	inter_long = (long1 - long2) * PI / 180.0;
	return (inter_long * colatB / (degree_dist(lat1, long1, lat2, long2) * PI / 180.0)) * 180.0 / PI;
}
//END

//compute a mean value recursively
float recursive_mean(float new_value, float old_mean, int n) {
	return (1 / (float) n) * (( (float) n-1) * old_mean + new_value);
}
//END

//compute a standard deviation recursively
float recursive_standard_deviation(float new_value, float current_mean, float prev_value, int n) {
	float temp=0.0, new_sd=0.0;
	if (n > 1) {
		temp = (prev_value * prev_value * ((float) n-2)) + ((float) n/( (float) n-1)) * (current_mean - new_value) * (current_mean - new_value);
		new_sd = sqrt(temp/((float) n-1));
	} else {
		fprintf(stderr,"Recursive_standard_deviation error: n must be greater than 1! n: %d\n",n);
	}
	return new_sd;
}
//END


//difference in time evaluation
int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1)
{
    long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
    result->tv_sec = diff / 1000000;
    result->tv_usec = diff % 1000000;

    return (diff<0);
}
//END

//print the time difference 
void timeval_print(struct timeval *tv)
{
    char buffer[30];
    time_t curtime;

    printf("%ld.%06ld", tv->tv_sec, tv->tv_usec);
    curtime = tv->tv_sec;
    strftime(buffer, 30, "%m-%d-%Y  %T", localtime(&curtime));
    printf(" = %s.%06ld\n", buffer, tv->tv_usec);
}
//END



/*-------------------MAIN----------------------*/
int main(int argc, char *argv[])
{
	/* local variables */
	int lag, i, j, k, l, npts_cor, n_cors=0;
	int *n_cor_done;
	int i_start = 0, j_start = 0;
	int stack_out_flag = 0, mean_out_flag = 1, sd_out_flag = 1; 
	int skip_flag_1 = 0, skip_flag_2 = 0;
	float *cor, *mean_cor, *sd_cor;
	float *tmpFloat;
	char db_name[100];
	char out_dir[100];
	char output_directory[300], str[300];
	char stack_out[100], mean_out[100], sd_out[100];
	char paramfile_name[100], buff[100];
//	char spect_filename_1[100], spect_filename_2[100];
	char header_filename_1[100], header_filename_2[100];
	char new_base_dir[100];
	struct stat st;
	FILE *db_file, *paramfile, *datafile;
	ANT_DB ant_db;
	SPECTRUM spect_1, spect_2, cor_spect;
	SAC_HD sac_header_1, sac_header_2, cor_header;
	double *ifft;
	fftw_complex *spect;
 	fftw_plan plan_backward;
//	struct timeval tvBegin, tvEnd, tvDiff;
//	float *all_data;

	/* check usage */
	if (argc != 2) {
		fprintf(stderr,"Usage: %s parameter_file\n",argv[0]);
		fprintf(stderr,"Paramter file format: \n");
		fprintf(stderr,"database_name\n");
		fprintf(stderr,"output_directory\n");
		fprintf(stderr,"lag_time (integer)\n");
		fprintf(stderr,"stack_out_flag mean_out_flag sd_out_flag\n");
		fprintf(stderr,"i_start j_start\n");
		fprintf(stderr,"new_base_dir\n");
		fprintf(stderr,"the x_out_flag can be 0 for do not output or 1 for output. Default is to output the mean and standard deviation\n");
		fprintf(stderr,"i_start and j_start are the integer values where the program left off when it started to slow down. To use the default enter 0 or leave blank\n");
		fprintf(stderr,"if the data has moved, you can point back to it with new_base_dir\n");
		exit(1);
	}	

	/* copy the parameter file name to a local variable */
	strcpy(paramfile_name,argv[1]);

	/*  open the parameter file */
	if (! (paramfile = fopen(paramfile_name,"r"))) {
		printf("Error, could not open file %s.\n",paramfile_name);
		exit(1);
	}

	/* read the parameter file */
	l=0;
	while (fgets(buff,100,paramfile)) {
		if (l == 0) {
			sscanf(buff,"%s",db_name);
			l++;
		} else if ( l == 1) {
			sscanf(buff,"%s",out_dir);
			l++;
		} else if ( l == 2) {
			sscanf(buff,"%d",&lag);
			l++;
		} else if (l == 3) {
			sscanf(buff,"%d %d %d",&stack_out_flag, &mean_out_flag, &sd_out_flag);
			l++;
		} else if (l ==4) {
			sscanf(buff,"%d %d", &i_start, &j_start);
			l++;
		} else if (l == 5) {
			sscanf(buff,"%s",new_base_dir);
			l++;
		}
	}

	/* open the database */
	if (!( db_file = fopen(db_name,"rb"))) {
		fprintf(stderr,"Error: %s not found\n",db_name);
		exit(2);
	}

	/* read the database */
	fread(&ant_db,sizeof(ant_db),1,db_file);
	
	/* close the database file */
	fclose(db_file);

	/* put in the new base directory */
	if (strcmp(new_base_dir,"")) {
		sprintf(ant_db.base_directory,"%s",new_base_dir);
	}

	/* compute the number of points to output into each correlation */
	npts_cor = 2 * lag + 1;

	/* first look into the database and estimate the computations required */
	k = 0;
	for (i=ant_db.n_stations-1; i>=0; i--) {
		k = k+i;
	}

	/* let the user know how many computations are necessary */
	printf("The expected maximum number of correlations per event is %d.\n",k);
	printf("The total number of events is %d.\nType ctrl+c to cancel\n",ant_db.n_events);

	/* array of integers for all correlation pairs. Initialized to 0. When a correlation is done for the pair, flag turns to 1. */
	if ((n_cor_done = malloc(sizeof(*n_cor_done) * k)) == NULL) {
		fprintf(stderr,"Error allocating n_cor_done\n");
		exit(1);
	}
	for (i=0; i<k; i++) {
		n_cor_done[i] = 0;
	}
	i = 0;

	/* allocate memory for floating point arrays*/
	if ((cor = malloc(sizeof(*cor) * npts_cor)) == NULL) {
		fprintf(stderr,"Error allocating cor\n");
		exit(1);
	}
	if ((mean_cor = malloc(sizeof(*mean_cor) * npts_cor)) == NULL) {
		fprintf(stderr,"Error allocating mean_cor\n");
		exit(1);
	}
	if ((sd_cor = malloc(sizeof(*sd_cor) * npts_cor)) == NULL) {
		fprintf(stderr,"Error allocating sd_cor\n");
		exit(1);
	}

	if ((tmpFloat = malloc(sizeof(*tmpFloat) * ant_db.npts_spectrum*2)) == NULL) {
		fprintf(stderr,"Error allocating tmpFloat\n");
		exit(1);
	}

	/* initialize the spectrums */
	initialize_spectrum(&spect_1,ant_db.npts_time_series);
	initialize_spectrum(&spect_2,ant_db.npts_time_series);
	initialize_spectrum(&cor_spect,ant_db.npts_time_series);

	/* initializations for fftw */
	if ((spect = fftw_malloc( sizeof ( *spect ) * spect_1.N)) == NULL) {
		fprintf(stderr,"Error allocating spect\n");
		exit(1);
	}
	if ((ifft = fftw_malloc(sizeof(*ifft) * ant_db.npts_time_series)) == NULL) {
		fprintf(stderr,"Error allocating ifft\n");
		exit(1);
	}
	plan_backward = fftw_plan_dft_c2r_1d(ant_db.npts_time_series,spect,ifft,FFTW_MEASURE);

	/* returning k to zero for future usage */
	k = 0; 

	/* hold a string variable to put th88e output correlations */
	sprintf(output_directory, "%s/%d/%02d/",out_dir,ant_db.event_table[i].year, ant_db.event_table[i].month);
	
	/* check if the directory exists and if it doesn't, creat it with mkdir -p to ensure the parents exist as well */
        if (stat(output_directory,&st) == -1) {
           fprintf(stderr,"dir: %s not found. Creating...\n",output_directory);
           sprintf(str,"mkdir -p %s\n",output_directory);
           system(str);
        }

	/* setup the starting points for j based on i */
	if (j_start == ant_db.n_stations - 1) {
		i_start++;
		j_start = 0;
	}

	/* attempt to read all the data stored in a single file */
/* in this version, we only have the memory for the current two stations single month in memory. we use fseek to find the appropriate point in the file 
	if ( (all_data = malloc(sizeof(*all_data) * 2 * ant_db.n_events * ant_db.npts_spectrum * ant_db.n_stations)) == NULL) {
		printf("Error, could not allocate memory for all_data\n");
		printf("Too many stations (%d) or too long of a spectrum (%d) . Give up and step away from the computer...\n",ant_db.n_stations, ant_db.npts_spectrum);
		exit(1);
	}
	if (! (datafile = fopen("all_data_for_correlations.dat","rb"))) {
		printf("Error, file all_data_for_correlations.dat not found\n");
		exit(1);
	}
	fread(all_data,sizeof(all_data), 2 * ant_db.n_events * ant_db.npts_spectrum * ant_db.n_stations, datafile);
	fclose(datafile);

	printf("Memory for all_data: %lg GB\n", sizeof(all_data) * 2 * ant_db.n_events * ant_db.npts_spectrum * ant_db.n_stations / (1024.0 * 1024.0 * 1024.0));
*/
        if (! (datafile = fopen("all_data_for_correlations.dat","rb"))) {
                printf("Error, file all_data_for_correlations.dat not found\n");
                exit(1);
        }
	int nvar = 0;

//when getting data for a given station - use fseek and then fread. fseek(filePointer, sizeof(var)*nvar,SEEK_SET)
//									then fread(&outputVariable, sizeof(var), n, filePointer)
//so make a new function, in which we pass a pointer to the array we want, a pointer to the file, and necessary information to get to the location (to compute nvar above)
// similar to:
//get_spectrum_from_data(all_data, &spect_1, k, ant_db.npts_spectrum*2,i, ant_db.n_events);

//get_spectrum_from_file(FILE *ff, SPECTRUM *out, int rec, int npts_rec, int station_num, int n_events, float *workingArray);
	/* loop over each station 1 */
	for (i=i_start; i<ant_db.n_stations - 1; i++) {
		/* let the user know which first station */
		printf("Working on net: %s station: %s component: %s\n",ant_db.station_table[i].network,ant_db.station_table[i].name,ant_db.station_table[i].component);

		/* setup the name of the header file */
		sprintf(header_filename_1,"%s/%d/%02d/headers/%s_%s_%s.SAC_header",ant_db.base_directory,ant_db.event_table[0].year, ant_db.event_table[0].month, ant_db.station_table[i].network,ant_db.station_table[i].name,ant_db.station_table[i].component);

		/* read the header for the first station */
		if (read_spectrum_header(header_filename_1, &sac_header_1) == 0) {
			printf("No data for station %s...skipping\n",ant_db.station_table[i].name);
			skip_flag_1 = 1;
		} else {
			skip_flag_1 = 0;
		}

		if (skip_flag_1 == 0) {
		/* loop over each second station */
			for (j=i + 1 + j_start; j<ant_db.n_stations; j++) {

				/* setup the name of the header file */
				sprintf(header_filename_2,"%s/%d/%02d/headers/%s_%s_%s.SAC_header",ant_db.base_directory,ant_db.event_table[0].year, ant_db.event_table[0].month,ant_db.station_table[j].network,ant_db.station_table[j].name,ant_db.station_table[j].component);
		
				/* read the header for the first station */
				if (read_spectrum_header(header_filename_2, &sac_header_2) == 0) {
					printf("skipping station %s_%s_%s\n",ant_db.station_table[j].network,ant_db.station_table[j].name, ant_db.station_table[j].component);
					skip_flag_2 = 1;
				} else {
					skip_flag_2 = 0;
				}

				/* incease n_cors - the index of n_cor_done keeps track of which correlation pair is being evaluated */
				n_cors++;
				if (skip_flag_2 == 0) {
					/* now loop over each event */
					for (k=0; k<ant_db.n_events; k++) {
						/* check that both stations are available. If they aren't, then skip */
						if (ant_db.station_table[i].on_off[k] == 1 && ant_db.station_table[j].on_off[k] == 1) {				
	
							/* get from the arrays the appropriate spectrums for the cross correlation */
//							get_spectrum_from_data(all_data, &spect_1, k, ant_db.npts_spectrum*2,i, ant_db.n_events);
//							get_spectrum_from_data(all_data, &spect_2, k, ant_db.npts_spectrum*2,j, ant_db.n_events);
//							get_spectrum_from_file(datafile, &spect_1, k, ant_db.npts_spectrum*2,i,ant_db.n_events, tmpFloat);
//							get_spectrum_from_file(datafile, &spect_2, k, ant_db.npts_spectrum*2,j,ant_db.n_events, tmpFloat);
							//similar to below, but when the memory is limited we need to repeated look up from the file 
/*
void get_spectrum_from_file(FILE *ff, SPECTRUM *out, int rec, int npts_rec, int station_num, int n_events, float *workingArray) {
        int i;
        int nvar;
        nvar = rec * npts_rec + station_num * npts_rec * n_events;
        //size of the stored data is float
        //working array is what we read into. it will contain the real and imaginary components. Once loaded, we parse it into the real and imaginary parts of the spectrum
        printf("%ld %d %d %d %d %d\n",sizeof(workingArray), rec, (int)floor(npts_rec/2+0.5), station_num, n_events,nvar);

        fseek(ff, sizeof(workingArray) * nvar, SEEK_SET);
        fread(workingArray, sizeof(workingArray), npts_rec, ff);
        for (i=0; i<floor(npts_rec/2+0.5); i++) {
                out->re[i] = workingArray[i];
                out->im[i] = workingArray[i+(int)floor(npts_rec/2+0.5)];
        }


        return;
}
*/

nvar = k * ant_db.npts_spectrum * 2 + i * ant_db.npts_spectrum*2*ant_db.n_events;
fseek(datafile, sizeof(float) * nvar, SEEK_SET);
fread(tmpFloat, sizeof(float), ant_db.npts_spectrum*2, datafile);
for (l=0; l<ant_db.npts_spectrum; l++) {
	spect_1.re[l] = tmpFloat[l];
	spect_1.im[l] = tmpFloat[l+ant_db.npts_spectrum];
}
//printf("%d ",nvar);
nvar = k * ant_db.npts_spectrum * 2 + j * ant_db.npts_spectrum*2*ant_db.n_events;
//printf("%d\n",nvar);
fseek(datafile, sizeof(float) * nvar, SEEK_SET);
fread(tmpFloat, sizeof(float), ant_db.npts_spectrum*2, datafile);
for (l=0; l<ant_db.npts_spectrum; l++) {
        spect_2.re[l] = tmpFloat[l];
        spect_2.im[l] = tmpFloat[l+ant_db.npts_spectrum];
}




	
							/* compute the correlation for this day */
							cross_correlate_rp(&spect_1,&spect_2,&cor_spect);
	
							/* compute ifft putting data into sig_out*/
							for (l=0; l<cor_spect.N; l++) {
							       spect[l][0] = (double) cor_spect.re[l];
							       spect[l][1] = (double) cor_spect.im[l];
							}
							fftw_execute(plan_backward);
					
							/* switch the array */
							cor[lag] = (float) ifft[0] / (float) ant_db.npts_time_series;
							for (l=1; l<(lag+1); l++) {
								cor[lag+l] = (float) ifft[l] / (float) ant_db.npts_time_series;
								cor[lag-l] = (float) ifft[ant_db.npts_time_series-l] / (float) ant_db.npts_time_series;
							}

/*
							for (l=0; l<npts_cor; l++) {
								if (!isnan(cor[l])) {
									fprintf(stderr,"%d NAN\n",l);
									exit(1);
								}
							}
*/

							/* avoid stacking nans */
							if (!isnan(cor[0])) {
								/* update the n_cor_done array */
								n_cor_done[n_cors]++;

								/* recursively compute the mean and standard deviation */
								if (n_cor_done[n_cors] == 1) {  //checking if this is the first correlation
									for (l=0; l<npts_cor; l++) { 
										mean_cor[l] = cor[l];  //just copy the computed correlation as the mean
										sd_cor[l] = 0.0;       //initialize the standard deviation to 0
									}
								} else { //otherwise we are computing the mean and standard deviation recursively.
									for (l=0; l<npts_cor; l++) {
										mean_cor[l] = recursive_mean(cor[l],mean_cor[l],n_cor_done[n_cors]);
										sd_cor[l] = recursive_standard_deviation(cor[l],mean_cor[l],sd_cor[l],n_cor_done[n_cors]);
									}
								} // updating the recursive mean
							}  //avoid nan's in stack
						} // done checking on off
					}  //done with all events
				} //skip flag 2
				/* update the cor_header to prepare the file writing */
				cor_header = sac_header_1;
				cor_header.npts = 2 * lag + 1;
	        	        cor_header.delta = sac_header_1.user2;
	       	         	cor_header.b = -1 * lag * cor_header.delta;
	                        cor_header.e = lag * cor_header.delta;
	                        cor_header.user0 = (float) n_cor_done[n_cors];
	                        cor_header.evla = sac_header_2.stla;
	                        cor_header.evlo = sac_header_2.stlo;
				cor_header.dist = km_dist(cor_header.stla, cor_header.stlo, cor_header.evla, cor_header.evlo);
	                        strcpy(cor_header.kevnm,ant_db.station_table[j].name);
				cor_header.az = compute_event_station_azimuth(cor_header.stla, cor_header.stlo, cor_header.evla, cor_header.evlo);
				cor_header.gcarc = degree_dist(cor_header.stla, cor_header.stlo, cor_header.evla, cor_header.evlo);
				strcpy(cor_header.kuser0,ant_db.station_table[j].network);
				strcpy(cor_header.kuser1,ant_db.station_table[j].component);


				/* prepare file names for the mean and standard deviation files */
				printf("writing output");
				sprintf(mean_out,"%s/%d/%02d/COR_%s_%s_%s.%s_%s_%s.SAC_mean",out_dir,ant_db.event_table[0].year, ant_db.event_table[0].month,ant_db.station_table[i].network,ant_db.station_table[i].name,ant_db.station_table[i].component, ant_db.station_table[j].network,ant_db.station_table[j].name,ant_db.station_table[j].component);
				sprintf(sd_out,"%s/%d/%02d/COR_%s_%s_%s.%s_%s_%s.SAC_sd",out_dir,ant_db.event_table[0].year, ant_db.event_table[0].month,ant_db.station_table[i].network,ant_db.station_table[i].name,ant_db.station_table[i].component, ant_db.station_table[j].network,ant_db.station_table[j].name,ant_db.station_table[j].component);
				sprintf(stack_out,"%s/%d/%02d/COR_%s_%s_%s.%s_%s_%s.SAC",out_dir,ant_db.event_table[0].year, ant_db.event_table[0].month,ant_db.station_table[i].network,ant_db.station_table[i].name,ant_db.station_table[i].component, ant_db.station_table[j].network,ant_db.station_table[j].name,ant_db.station_table[j].component);

				/* write the mean, standard deviation, and stack out based on user's flags */
				if (mean_out_flag == 1 && !isnan(mean_cor[0])) {
					write_sac(mean_out,mean_cor,&cor_header);
				}
	
				if (sd_out_flag == 1 && !isnan(sd_cor[0])) {
					write_sac(sd_out,sd_cor,&cor_header);
				}
	
				if (stack_out_flag == 1 && !(isnan(mean_cor[0])) && cor_header.user0 > 0.0) {
					for (l=0; l<npts_cor; l++) {
						mean_cor[l] = mean_cor[l] * cor_header.user0;
					}
					write_sac(stack_out,mean_cor,&cor_header);
				}
			} // done with station 2
		} //done with skip flag 1 - for the whole month having no data (not sure it should be in db)
	} // done with station 1

	fclose(datafile);
/*
	// destroy the fftw plan backwards and free the fftw malloc 
	fftw_destroy_plan(plan_backward);
	fftw_free(spect);
	fftw_free(ifft);

	// free regular memory 
	free(cor);
	free(mean_cor);
	free(sd_cor);
	free(n_cor_done);
	free(tmpFloat);

	// free spectrums 
	free(spect_1.re);
	free(spect_1.im);
	free(spect_2.re);
	free(spect_2.im);
	free(cor_spect.re);
	free(cor_spect.im);
*/
	/* done */
	return 0;
}

