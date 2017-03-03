#include <stdlib.h>
#include <stdio.h>
#include "mysac.h"

/*--------------------------------------------------------------------------*/
void write_sac (char *fname, float *sig, SAC_HD *SHD)
/*--------------------------------------------------------------------------*/
{
  FILE *fsac;
  int i;
  if((fsac = fopen(fname, "wb"))==NULL) {
    fprintf(stderr,"write_sac: Could not open %s to write\n", fname);
  }
  else {

    if ( !SHD ) {
      SHD = &SAC_HEADER;
    }

    SHD->iftype = (long)ITIME;
    SHD->leven = (long)TRUE;
    SHD->lovrok = (long)TRUE;
    SHD->internal4 = 6L;
    SHD->depmin = sig[0];
    SHD->depmax = sig[0];

    for ( i = 0; i < SHD->npts ; i++ ) {
      if ( SHD->depmin > sig[i] ) {
        SHD->depmin = sig[i];
      }
      if ( SHD->depmax < sig[i] ) {
        SHD->depmax = sig[i];
      }
    }

    fwrite(SHD,sizeof(SAC_HD),1,fsac);
    fwrite(sig,sizeof(float),(int)(SHD->npts),fsac);

    fclose (fsac);
  }
}

