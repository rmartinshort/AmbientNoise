/* standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

/* user headers */
#include "ant_db.h"
#include "mysac.h"

#define NPTS_SAC_FILE_MAX 100000

/* function prototypes */
extern SAC_HD *read_sac (char *,float *, SAC_HD *, int );

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



int main (int argc, char *argv[]) {
	int skip_flag = 0, n_sac=0, n_killed=0;
	char buff[100], list_file_name[100], sac_file_name[100];
	char outfile_name[110];
	FILE *list_file, *outfile;
	float *signal;
	SAC_HD sac_header;
	
	if (argc != 2) {
		printf("Usage: %s list_of_sac_files\n",argv[0]);
		exit(1);
	}

	sscanf(argv[1], "%s", list_file_name);

	if (! (list_file = fopen(list_file_name,"r"))) {
		printf("Error, file %s not found\n", list_file_name);
		exit(1);
	}

	signal = malloc(sizeof(signal) * NPTS_SAC_FILE_MAX);

	sprintf(outfile_name,"%s.out",list_file_name);
	outfile = fopen(outfile_name,"w");

	while (fgets(buff,100,list_file)) {
		sscanf(buff,"%s",sac_file_name);
		if (! (read_sac(sac_file_name,signal,&(sac_header),NPTS_SAC_FILE_MAX))) {
			printf("Error file %s not found\n",sac_file_name);
			skip_flag = 1;
		} else {
			skip_flag = 0;
		}
		
		if (skip_flag == 0) {
			n_sac++;
			if (sac_header.depmin == 0.0000 && sac_header.depmax == 0.000000) {
				n_killed++;
				fprintf(outfile,"%s\n",sac_file_name);
			}
		}
	}
	fclose(outfile);
	fclose(list_file);

	printf("out of %d traces, %d were dead (%f)%%\n",n_sac, n_killed, (float) n_killed / (float) n_sac * 100.0);
	free(signal);
				
	return 0;
}


