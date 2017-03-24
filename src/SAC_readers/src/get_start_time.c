/*
get_start_time
program reads in a sac file and returns the following header values:
nzyear nzjday nzhour nzmin nzsec nzmsec
*/

//standard headers
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

//user header
#include "mysac.h"

//define maximum number of points in a sac file
#define NPTS_SAC_FILE_MAX 200000

//function prototypes
//read_sac_d
extern SAC_HD *read_sac_d(char *, double *, SAC_HD *, int);


/*-----------------MAIN----------------------*/
int main(int argc, char *argv[]) {
	/* local variables */
	char sac_file_name[100];
	double *signal;
	SAC_HD sac_header;

	/* check usage */
	if (argc != 2) {
		fprintf(stderr,"Usage: %s sac_file\n",argv[0]);
		fprintf(stderr,"%s returns the timing information as:\n",argv[0]);
		fprintf(stderr,"nzyear nzjday nzhour nzmin nzsec nzmsec\n");
		fprintf(stderr,"use awk to get the specific value you want\n");
		exit(1);
	}

	/* allocate memory */
	signal = malloc(sizeof(signal) * NPTS_SAC_FILE_MAX);

	/* initialize the header */
//	sac_header = sac_null;

	/* copy the arguments */
	strcpy(sac_file_name,argv[1]);

	/* read sac file */
	if (!read_sac_d(sac_file_name,signal,&(sac_header),NPTS_SAC_FILE_MAX)) {
		fprintf(stderr,"Error: could not open %s with read_sac\n", sac_file_name);
		exit(2);
	}
	
	/* print the header information */
	printf("%d %d %d %d %d %d\n",sac_header.nzyear, sac_header.nzjday, sac_header.nzhour, sac_header.nzmin, sac_header.nzsec, sac_header.nzmsec);

	/* free memory */
	free(signal);	
	


	return 0;
}



