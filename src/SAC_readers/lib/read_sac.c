//read_sac_d
#include "mysac.h"
//standard headers
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif



//provides a cast to allow reading a sac file and returning a double precision array
/*--------------------------------------------------------------------------*/
        SAC_HD *read_sac_d (char *fname, double *sig_d, SAC_HD *SHD, int nmax)
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
         return NULL;
        }


//	SHD = &sac_null;
        if ( !SHD ) SHD = &SAC_HEADER;

         fread(SHD,sizeof(SAC_HD),1,fsac);

         if ( SHD->npts > nmax )
         {
          fprintf(stderr,
           "ATTENTION !!! in the file %s the number of points is limited to %d\nNPTS found: %d\n",fname,nmax, SHD->npts);

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
        return SHD;
}
//////END




