/*
pack_spectrums.c
program makes a database to use for the cross correlation
usage:
pack_spectrums parameter_file

ANT_db format in the libantrp.h header file:
#define MAX_STATIONS 1000
#define MAX_EVENTS 32

typedef struct EVENT {
        int year;
        int month;
        int day;
        int jday;
} EVENT;

typedef struct STATION {
        char name[5];
        char component[4];
        char network[3];
        int on_off[MAX_EVENTS];
} STATION;

typedef struct ANT_DB {
        int n_events;
        int n_stations;
        STATION station_table[MAX_STATIONS];
        EVENT event_table[MAX_EVENTS];
} ANT_DB;


all of the event information can be gotten from the filestructure. 

//edit 7.19.2010
Added subroutines:
read_sac()
write_spectrum_header()
write_spectrum_data()
added headers:
mysac.h



*/

/* standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif
#include <sys/stat.h>

/* user headers */
#include "ant_db.h"
#include "mysac.h"

#define NPTS_SAC_FILE_MAX 43201 

/* function prototypes */
extern SAC_HD *read_sac (char *,float *, SAC_HD *, int );
void write_spectrum_header(char *out_dir, SAC_HD *shd);
void write_spectrum_data(char *out_dir, float *signal, int npts);
void append_spectrum_data(float *real_data, float *imag_data, float *big_array, int event, int npts);

/* subroutines */

void append_spectrum_data(float *real_data, float *imag_data, float *big_array, int event, int npts) {
	int i, r_i, i_i;

//      (sig_re,big_data,i,0, sac_hd_r.npts)
//	i + rec * npts_rec + station_num * npts_rec * n_events

	for (i=0; i<npts; i++) {
//		big_array[i+event*component*npts] = new_data[i];
		r_i = i + event * npts * 2.0;
		i_i = r_i + npts;
		big_array[r_i] = real_data[i];
		big_array[i_i] = imag_data[i];
	}

	return;
}


//write_spectrum_header
void write_spectrum_header(char *out_dir, SAC_HD *shd) {

	char output_file_name[100];
	FILE *outputfile;
	char net[3], station[6], component[4];

	sscanf(shd->knetwk,"%s",net);
	sscanf(shd->kstnm,"%s",station);
	sscanf(shd->kcmpnm,"%s",component);

	sprintf(output_file_name,"%s/%s_%s_%s.SAC_header",out_dir,net,station,component);

	outputfile = fopen(output_file_name, "wb");
	fwrite(shd,sizeof(SAC_HD),1,outputfile);
	fclose(outputfile);


	return;
}

//write_spectrum_data
void write_spectrum_data(char *out_dir, float *signal, int npts) {

	char output_file_name[100];
	FILE *outputfile;
//	char net[3], station[6], component[4];

//	sscanf(shd->knetwk,"%s",net);
//	sscanf(shd->kstnm,"%s",station);
//	sscanf(shd->kcmpnm,"%s",component);

//	sprintf(output_file_name,"%s/%s_%s_%s.SAC_spectrum",out_dir,net,station,component);
	sprintf(output_file_name,"all_data_for_correlations.dat");

	outputfile = fopen(output_file_name,"ab");
	
	fwrite(signal,sizeof(float),npts,outputfile);

	fclose(outputfile);
	
	return;
}	


//END

//read_sac
//provides a cast to allow reading a sac file and returning a double precision array
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
//      fprintf(stderr,"read_sac_d: ERROR file %s not found!\n",fname);
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



int main(int argc, char *argv[])
{
	/* local variable declaration */
	int i,l=0;
	int delete_flag = 0;
	int year, month, start_month_day, end_month_day, start_jday, end_jday;
	int counter_found = 0, counter_looked = 0, counter_not_found = 0;
	int m, *found_flag, n_temp;
	int npts_sac_file;
	float *sig_re, *sig_im, *big_data;
	char paramfile_name[100], stationlist_name[100], output_name[100], base_dir[100];
	char buff[100], temp_name_re[100],temp_name_im[100], str[100];
	char outdir[100];
	FILE *paramfile, *stationlist, *output;	
	struct stat st;
	ANT_DB ant_db;
	SAC_HD sac_hd_r, sac_hd_i;

	/* check the usage */
	if (argc!=2) {
		fprintf(stderr,"USAGE: %s parameter_file_name\n", argv[0]);
		fprintf(stderr,"parameter file format: \n");
		fprintf(stderr,"year month(numerical 1-12)\n");
		fprintf(stderr,"start_month_day(1-31) end_month_day(1-31)\n");
		fprintf(stderr,"start_jday(1-366) end_jday(1-366)\n");
		fprintf(stderr,"station_list_file_name\n");
		fprintf(stderr,"base_directory_with_sac_waveforms\n");
		fprintf(stderr,"output_database_name\n");
		fprintf(stderr,"delete_flag - 0 will not delete originals, while 1 will delete original sac files\n");
		fprintf(stderr,"number_of_points_in_each_sac_file (43201 for 1 day of LH)\n\n\n");
		fprintf(stderr,"The station list file contains all the possible stations used.\n"); 
		fprintf(stderr,"First column contains network, second contains station name, and the third the channel.\n\n");
		fprintf(stderr,"the base directory is the full path to a location containing sac files\n");
		fprintf(stderr,"The following chart shows the directory structure assumed:\n");
		fprintf(stderr,"                       BASE_DIR/                                          \n");
		fprintf(stderr,"       year_1/      year_2/    .........................................  \n");
		fprintf(stderr," month_1/  month_2/ (expressed as a number ie 1, 2, 3, 4, 5.............  \n");
		fprintf(stderr,"1/ 2/ 3/ 4/ 5/ 6/ 7/ 8/ 9/ 10/ .........................................  \n");
		fprintf(stderr,"network_station_component.SAC_re & network_station_component.SAC_im  ...  \n\n\n");
		fprintf(stderr,"May in the future change the functionability to be start_jday to end_jday \n");	
		exit(1);
	}

	/* copy the name of the paramter file to a variable and open it */
	strcpy(paramfile_name,argv[1]);
	if (! (paramfile = fopen(paramfile_name,"r"))) {
		fprintf(stderr,"Error: could not open file: %s\n",paramfile_name);
		exit(2);
	}	

	/* read the parameter file one line at a time */
	while (fgets(buff,100,paramfile)) {
		if (l==0) {		
			sscanf(buff,"%d %02d\n",&year, &month);
			l++;
		} else if (l==1) {
			sscanf(buff,"%02d %02d\n",&start_month_day, &end_month_day);
			l++;
		} else if (l==2) {
			sscanf(buff,"%d %d\n",&start_jday, &end_jday);
			l++;
		} else if (l==3) {
			sscanf(buff,"%[A-z,0-9,.,_,/]\n",stationlist_name);
			l++;
		} else if (l==4) {
			sscanf(buff,"%[A-z,0-9,.,_,/]\n",base_dir);
			l++;
		} else if (l==5) {
			sscanf(buff,"%[A-z,0-9,.,_,/]\n",output_name);
			l++;	
		} else if (l == 6) {
			sscanf(buff,"%d",&delete_flag);
			l++;
		} else if (l == 7) {
			sscanf(buff,"%d",&npts_sac_file);
			l++;
		}
	}

	/* close the paramter file */
	fclose(paramfile);

	/* fill the event table */
	ant_db.n_events = end_jday - start_jday + 1;
//	ant_db.n_events = end_month_day - start_month_day + 1;
	for (i=0; i<ant_db.n_events; i++) {
		ant_db.event_table[i].year = year;
		ant_db.event_table[i].month = month;
		ant_db.event_table[i].day = start_month_day + i;
		ant_db.event_table[i].jday = start_jday + i;
	}
	i=0;

	/* fill the station table from the station list file */
	/* first open the station list file */	
	if (! (stationlist = fopen(stationlist_name,"r"))) {
	        fprintf(stderr,"Error: could not open file: %s\n",stationlist_name);
	        exit(3);
	}
		
	/* Read over the stationlist file which has the format: network, station, channel */
	while (fgets(buff,100,stationlist)) {
		sscanf(buff,"%s %s %s\n",ant_db.station_table[i].network,ant_db.station_table[i].name, ant_db.station_table[i].component);
		i++;
	}
	ant_db.n_stations = i;
	
	/* close the station file */
	fclose(stationlist);

	/* allocate memory for the found flag array */
	found_flag = malloc(sizeof(found_flag) * ant_db.n_stations);
	for (i=0; i<ant_db.n_stations; i++) {
		found_flag[i] = 0;
	}

	/* allocate memeory for the signals */
	sig_re = malloc(sizeof(sig_re) * npts_sac_file);
	sig_im = malloc(sizeof(sig_im) * npts_sac_file);
	big_data = malloc(sizeof(big_data) * 2 * ant_db.n_events * npts_sac_file);

	/* if the program is setup to delete files, warn the user */
	if (delete_flag == 1) {
		printf("!! Warning program is currently set to remove original .SAC files after making files for the correlation.\nEnter 1 to continue to delete files or 0 to conserve files\n");
		scanf("%d",&delete_flag);
		if (delete_flag == 1) {
			printf("Will remove .SAC files. This cannot be undone at this point!\n");
		} else {
			printf("Will not remove raw .SAC files. This will lead to extra hard drive space usage\n");
		}
	}

	/* fill top level components of database */
	sprintf(ant_db.base_directory,"%s",base_dir);
	ant_db.npts_spectrum = npts_sac_file;
	ant_db.npts_time_series = (npts_sac_file - 1) * 2;

/* check for the actual availability of each file */
//This component must be tied to a filestructure. 
//However all it does is fill an array which can be checked in the
// correlation routine to see if a station is available for cross correlation. 
//This check could alternatively be done at run time of the correlation routine
	n_temp = npts_sac_file;
	sac_hd_r.npts = n_temp;
	sac_hd_i.npts = n_temp;

	/* the directory to put the spectrums and headers */
	sprintf(outdir,"%s/%d/%02d/headers",base_dir,year,month);

	 /* check if the directory exists and if it doesn't, creat it with mkdir -p to ensure the parents exist as well */
        if (stat(outdir,&st) == -1) {
           fprintf(stderr,"dir: %s not found. Creating...\n",outdir);
           sprintf(str,"mkdir -p %s\n",outdir);
           system(str);
        }


	for (l=0; l<ant_db.n_stations; l++) {
		for (i=0; i<ant_db.n_events; i++) {
			sprintf(temp_name_re,"%s/%d/%02d/%02d/%s_%s_%s.SAC_re",base_dir,year,month,ant_db.event_table[i].day,ant_db.station_table[l].network,ant_db.station_table[l].name,ant_db.station_table[l].component);
                        sprintf(temp_name_im,"%s/%d/%02d/%02d/%s_%s_%s.SAC_im",base_dir,year,month,ant_db.event_table[i].day,ant_db.station_table[l].network,ant_db.station_table[l].name,ant_db.station_table[l].component);
			counter_looked++;
//			if ((temp_file = fopen(temp_name,"r"))) {
			if (! (read_sac(temp_name_re,sig_re,&(sac_hd_r),NPTS_SAC_FILE_MAX))) {
				//printf("file %s found\n",temp_name);
				ant_db.station_table[l].on_off[i] = 0;
				printf("file %s not found\n",temp_name_re);
//				fclose(temp_file);
				counter_not_found++;
				for (m=0; m<n_temp; m++) {
					sig_re[m] = 0.0;
					sig_im[m] = 0.0;
				}
				sprintf(sac_hd_r.knetwk,"%s",ant_db.station_table[l].network);
				sprintf(sac_hd_r.kstnm,"%s",ant_db.station_table[l].name);
				sprintf(sac_hd_r.kcmpnm,"%s",ant_db.station_table[l].component);

                                sprintf(sac_hd_i.knetwk,"%s",ant_db.station_table[l].network);
                                sprintf(sac_hd_i.kstnm,"%s",ant_db.station_table[l].name);
                                sprintf(sac_hd_i.kcmpnm,"%s",ant_db.station_table[l].component);

//				write_spectrum_data(outdir,sig_re,&(sac_hd_r));
//                              write_spectrum_data(outdir,sig_im,&(sac_hd_i));
				append_spectrum_data(sig_re, sig_im, big_data, i, sac_hd_r.npts);
			} else {
				read_sac(temp_name_im,sig_im,&(sac_hd_i),NPTS_SAC_FILE_MAX);
				counter_found++;
				n_temp = sac_hd_i.npts;
				ant_db.station_table[l].on_off[i] = 1;
				if (i == 0 || found_flag[l] == 0) {
					found_flag[l] = 1;
					write_spectrum_header(outdir,&(sac_hd_r));
				}
//				write_spectrum_data(outdir,sig_re,&(sac_hd_r));
//				write_spectrum_data(outdir,sig_im,&(sac_hd_i));
                                append_spectrum_data(sig_re, sig_im, big_data, i, sac_hd_r.npts);
			}
			//fclose(temp_file);
			if (delete_flag == 1) {
				sprintf(str,"rm -f %s %s\n",temp_name_re, temp_name_im);
				system(str);
			}
		}
		write_spectrum_data(outdir,big_data, 2 * ant_db.n_events * npts_sac_file);
	}

	printf("out of %d files looked for, %d were found and %d were not found\n",counter_looked,counter_found,counter_not_found);

	/* write the database structure to a file */
	output = fopen(output_name,"wb");
	fwrite(&ant_db,sizeof(ant_db),1,output);
	fclose(output);	

	/* free up memory like a good citizen */
	free(found_flag);
	free(sig_re);
	free(sig_im);
	free(big_data);

	return 0;
}


