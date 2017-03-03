/*
stack_rp_with_io.c

given a list of networks, stations, and channels
      a start month/year and end month/year
      an output directory for the stacked correlations
looks for all the files in a standard file structure and stacks appropriately
outputs the stacked waveforms in the specified directory as well as a list of the files stacked (for the cut to symmetric component)

Subroutines called:
read_sac_d
write_sac_d
get_file_name_from_full_path


*/

/* standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

/* user headers */
#include "mysac.h"
#include "ant_db.h"

/* global constants */
#define NPTS_SAC_FILE_MAX 20000   //This value is lower than previous declarations because the correlation files are assumed to have less lag time than a full day


/*-------------subroutines--------------*/
//declarations
int read_sac_d (char *, double *, SAC_HD *, int );
extern void write_sac_d (char *, double *, SAC_HD *);
void get_filename_from_full_path(char *in_string, char *out_string);


//read_sac_d
//provides a cast to allow reading a sac file and returning a double precision array
/*--------------------------------------------------------------------------*/
 int read_sac_d (char *fname, double *sig_d, SAC_HD *SHD, int nmax)
/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/
{
 FILE *fsac;
 float *sig;
 int i;
/*..........................................................................*/
        fsac = fopen(fname, "rb");
        if ( !fsac )
        {
         //fclose (fsac);
//      fprintf(stderr,"read_sac_d: ERROR file %s not found!\n",fname);
         return 0;
        }

        if ( !SHD ) SHD = &SAC_HEADER;

         fread(SHD,sizeof(SAC_HD),1,fsac);

         if ( SHD->npts > nmax )
         {
          fprintf(stderr,
           "ATTENTION !!! in the file %s the number of points is limited to %d\n",fname,nmax);

          SHD->npts = nmax;
         }

        sig = malloc(sizeof(sig) * SHD->npts);

         fread(sig,sizeof(float),(int)(SHD->npts),fsac);

       for (i=0; i<SHD->npts; i++) {
          sig_d[i] = (double) sig[i];
        }

        free(sig);

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
        return 1;
}
//////END

//write_sac_d
//provides a cast to write sac files with double precision input arrays
void write_sac_d (char *fname, double *sig_d, SAC_HD *SHD)
/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/
{
 FILE *fsac;
 int i;
 float *sig;
/*..........................................................................*/
        fsac = fopen(fname, "wb");

        if ( !SHD ) SHD = &SAC_HEADER;
        sig = malloc(SHD->npts * sizeof(sig));
        for (i=0; i<SHD->npts; i++) {
                sig[i] = (float) sig_d[i];
        }

        SHD->iftype = (long)ITIME;
        SHD->leven = (long)TRUE;

        SHD->lovrok = (long)TRUE;
        SHD->internal4 = 6L;

  /*+++++++++++++++++++++++++++++++++++++++++*/
     SHD->depmin = sig[0];
     SHD->depmax = sig[0];

   for ( i = 0; i < SHD->npts ; i++ )
   {
    if ( SHD->depmin > sig[i] ) SHD->depmin = sig[i];
    if ( SHD->depmax < sig[i] ) SHD->depmax = sig[i];
   }

         fwrite(SHD,sizeof(SAC_HD),1,fsac);
         fwrite(sig,sizeof(double),(int)(SHD->npts),fsac);

        fclose(fsac);
        free(sig);
}
////END


//get_filename_from_full_path
void get_filename_from_full_path(char *in_string, char *out_string) {
        int i, j=0;
        char buff[100];

        for (i=0; i<strlen(in_string); i++) {
                if (in_string[i] == '/') {
                        j=0;
                } else {
                        buff[j] = in_string[i];
                        j++;
                }
        }

        strcpy(out_string,buff);

        return;
}
///END




/*---------------MAIN---------------*/
int main(int argc, char *argv[])
{
	/* local variables */
	int i, j, lag_1, lag_2, l, k, m, y, ms, me, ncors=0;
	int start_month, start_year;
	int end_month, end_year;
	int nstations, other_flag=0;
	int skip_flag = 0, is_first_sac_file_flag = 0;
	char paramFileName[100];
	char list_file_name[100], ff_name[300], buff_list[300], sac_file_name[300], buff[300];
	char output_directory[300], out_file_name[300], out_name_stub[300];
	char station[6], event[6], component[4];
	char event_net[3], event_component[4], station_net[3];
	char first_part[100], second_part[100];
	double *signal, *temp;
	FILE *list_file, *temp_file, *paramFile;
	SAC_HD sac_header, temp_header;
	STATION all_cor_stations[MAX_STATIONS];

	/* check usage */
	if (argc != 2) {
                fprintf(stderr,"Usage: %s parameterfile \n",argv[0]);
		fprintf(stderr,"parameterfile format:\n");
		fprintf(stderr,"station_list [format = network station channel]\n");
		fprintf(stderr,"start_month start_year\n");
		fprintf(stderr,"end_month end_year\n");
		fprintf(stderr,"output_directory\n");
                exit(1);
        }

	/* allocate memory */
	signal = malloc(sizeof(signal) * NPTS_SAC_FILE_MAX);
	temp = malloc(sizeof(temp) * NPTS_SAC_FILE_MAX);

	/* copy the argument to a local variable */
//	strcpy(list_file_name,argv[1]);
//	strcpy(output_directory,argv[2]);
	sscanf(argv[1],"%s",paramFileName);
	if (( paramFile = fopen(paramFileName,"r")) == NULL) {
		fprintf(stderr,"Error, file %s not found\n",paramFileName);
		exit(1);
	}

	l=0;
	while (fgets(buff,100,paramFile)) {
		if (l == 0) {
			sscanf(buff,"%s",list_file_name);
			l++;
		} else if (l == 1) {
			sscanf(buff,"%02d %d", &start_month, &start_year);
			l++;
		} else if (l == 2) {
			sscanf(buff,"%02d %d", &end_month, &end_year);
			l++;
		} else if (l == 3) {
			sscanf(buff,"%s", output_directory);
			l++;
		}
	}

	fclose(paramFile);

	/* try to open the station list file */
	if (! (list_file = fopen(list_file_name,"r"))) {
                printf("Error: file %s not found\n", list_file_name);
                exit(1);
        }

	/* loop over each list in the first list file to fill the station list structure */	
	printf("Filling station structure\n");
	l=0;
	while(fgets(buff_list,300,list_file)) {
        	sscanf(buff_list,"%s %s %s",station_net, station, component);

		sprintf(all_cor_stations[l].name,"%s",station);
		sprintf(all_cor_stations[l].component,"%s",component);
		sprintf(all_cor_stations[l].network,"%s",station_net);
		l++;
	}		
	fclose(list_file);
	printf("Done filling station structure. Beginning stacking\n");

	/* capture the total number of stations */
	nstations = l;

	if (( temp_file = fopen("filelist_stack","w")) == NULL) {
		fprintf(stderr,"PNWED\n");
		exit(1);
	}

	/* loop over each first station.
	   first, have a variable for the first part of the correlation name:
           COR_net_station_component.
           then loop over each second station making the second part
           net_station_component.SAC
           we can now make the output name as:
           outputdir/{firstpart}{secondpart}
           loop through time incrementing the year once the month gets past 12
           finalize the input names as
           month/year/{firstpart}{secondpart}
           This only works with the default filestructure and naming conventions.
           You are now warned.
        */
	for (l=0; l<nstations-1; l++) {
		sprintf(first_part,"COR_%s_%s_%s.",all_cor_stations[l].network, all_cor_stations[l].name, all_cor_stations[l].component);
		printf("working on %s %s %s\n",all_cor_stations[l].network, all_cor_stations[l].name, all_cor_stations[l].component);
		for (k=l+1; k<nstations; k++) {
			sprintf(second_part,"%s_%s_%s.SAC", all_cor_stations[k].network, all_cor_stations[k].name, all_cor_stations[k].component);
			
			sprintf(out_file_name,"%s/%s%s",output_directory,first_part,second_part);
			/* prevent redoing a correlation */
			if ( (read_sac_d(out_file_name,signal,&(sac_header),NPTS_SAC_FILE_MAX) == 0) ) {
				is_first_sac_file_flag = 1;
				ncors = 0;

				for (y=start_year; y<=end_year; y++) {
					ms = 1;
					me = 12;
					if (y == start_year) {
						ms = start_month;
					}
					if (y == end_year) {
						me = end_month;
					}
					for (m = ms; m<= me; m++) {
						sprintf(ff_name,"%d/%02d/%s%s",y,m,first_part,second_part);
//						printf("%s\n",ff_name);
						if (is_first_sac_file_flag == 1) {
							if ( ( read_sac_d(ff_name,signal,&(sac_header),NPTS_SAC_FILE_MAX) == 0) ) {
								skip_flag = 1;
							} else {
								skip_flag = 0;
								is_first_sac_file_flag = 0;
								other_flag = 1;
								ncors++;
								if (isnan(sac_header.depmin) || isnan(sac_header.depmax || (sac_header.depmin == 0.0000 && sac_header.depmax == 0.000000) )) {
									skip_flag = 1;
									is_first_sac_file_flag = 1;
									ncors--;
								}
							}
						} else {
							if ( ( read_sac_d(ff_name,temp,&(temp_header),NPTS_SAC_FILE_MAX) == 0) ) {
        	                                                skip_flag = 1;
        	                                        } else {
								ncors++;
               		                                        skip_flag = 0;
								other_flag = 0;
								if (isnan(temp_header.depmin) || isnan(temp_header.depmax || (temp_header.depmin == 0.0000 && temp_header.depmax == 0.000000))) {
									skip_flag = 1;
									ncors--;
								}
	                                                }
						}
						if (skip_flag == 0 && is_first_sac_file_flag == 0 && other_flag == 0) {
							//Lets assume the sample rate is the same, but make a consideration if the lag time is different
			                                //define the lag time
			                                lag_1 = (sac_header.npts - 1) / 2;
			                                lag_2 = (temp_header.npts - 1) / 2;
			                                //first define the central point
			                                signal[lag_1] = signal[lag_1] + temp[lag_2];
			                                //choose the lower lag
			                                if (lag_1 == lag_2) {
			                                        for (j=0; j<sac_header.npts; j++) {
			                                                signal[j] = signal[j] + temp[j];
			                                        }
			                                } else if (lag_1 < lag_2) {
			                                        for (j=1; j<(lag_1+1); j++) {
			                                                signal[lag_1+j] = signal[lag_1+j]+temp[lag_2+j];
			                                                signal[lag_1-j] = signal[lag_1-j]+temp[lag_2-j];
			                                        }
			                                } else {
			                                        for (j=1; j<(lag_2+1); j++) {
			                                                signal[lag_1+j] = signal[lag_1+j]+temp[lag_2+j];
			                                                signal[lag_1-j] = signal[lag_1-j]+temp[lag_2-j];
			                                        }
			                                }
							//increase the user0 variable
			                                sac_header.user0 = sac_header.user0 + temp_header.user0;
						}
					}  //end months
				}  // end years
				//output the stacked sac file 
				if (ncors > 0 && sac_header.user0 > 0.0) {
					write_sac_d(out_file_name,signal,&(sac_header));
					fprintf(temp_file,"%s%s  \t%3.0f\n",first_part,second_part, sac_header.user0);
				}
			} //preventing redo a stack
		}  //station 2
	}  // station 1
	
	fclose(temp_file);
	free(signal);
	free(temp);

	return 0;
}


