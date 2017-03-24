//write_sac
#include "mysac.h"
//standard headers
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
        void write_sac_d (char *fname, double *sig_d, SAC_HD *SHD)
/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/
{
 FILE *fsac;
 int i;
 float *sig;
/*..........................................................................*/
        fsac = fopen(fname, "wb");

//	if ( !SHD ) SHD = &sac_null;
//      if ( !SHD ) SHD = &SAC_HEADER;
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
	 fwrite(sig,sizeof(float),(int)(SHD->npts),fsac);

        fclose(fsac);
        free(sig);
}
////END

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
        void write_sac (char *fname, float *sig, SAC_HD *SHD)
/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/
{
 FILE *fsac;
 int i;
/*..........................................................................*/
        fsac = fopen(fname, "wb");

//        if ( !SHD ) SHD = &sac_null;
//        if ( !SHD ) SHD = &SAC_HEADER;

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
         fwrite(sig,sizeof(float),(int)(SHD->npts),fsac);

        fclose(fsac);
}



