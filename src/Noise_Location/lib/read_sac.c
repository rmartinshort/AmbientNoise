#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "mysac.h"
#include "sac_db.h"

/*--------------------------------------------------------------------------*/
SAC_HD *read_sac (char *fname, float *sig, SAC_HD *SHD, int nmax)  /* nmax was originally be long */
/*--------------------------------------------------------------------------*/
/* function to read sac files given the name, fname. The function outputs the time signal to the pointer sig, fills the header SHD, if the signal has fewer than nmax points */
{
  FILE *fsac;

  if((fsac = fopen(fname, "rb")) == NULL) {
    fprintf(stderr,"read_sac: Could not open %s\n", fname);
    return NULL;
  }

  if ( !fsac ) {
    fprintf(stderr,"file %s not find\n", fname);
    return NULL;
  }

  if ( !SHD ) SHD = &SAC_HEADER;

  fread(SHD,sizeof(SAC_HD),1,fsac);

/* print the sac header variables (this was too much work to delete...so leave it for future debugging if needed) */
/*  printf("(read_sac) the top row is: %f, %f, %f, %f, %f.\n",SHD->delta, SHD->depmin, SHD->depmax, SHD->scale, SHD->odelta);
  printf("(read_sac) the second row is: %f, %f, %f, %f, %f.\n",SHD->b, SHD->e, SHD->o, SHD->a, SHD->internal1 );
  printf("(read_sac) the third row is: %f, %f, %f, %f, %f.\n",SHD->t0, SHD->t1, SHD->t2, SHD->t3, SHD->t4);
  printf("(read_sac) the fourth row is: %f, %f, %f, %f, %f.\n",SHD->t5, SHD->t6, SHD->t7, SHD->t8, SHD->t9 );
  printf("(read_sac) the fifth row is: %f, %f, %f, %f, %f.\n",SHD->f, SHD->resp0, SHD->resp1, SHD->resp2, SHD->resp3);
  printf("(read_sac) the sixth row is: %f, %f, %f, %f, %f.\n",SHD->resp4, SHD->resp5, SHD->resp6, SHD->resp7, SHD->resp8 );
  printf("(read_sac) the seventh row is: %f, %f, %f, %f, %f.\n",SHD->resp9, SHD->stla, SHD->stlo, SHD->stel, SHD->stdp);
  printf("(read_sac) the eighth row is: %f, %f, %f, %f, %f.\n",SHD->evla, SHD->evlo, SHD->evel, SHD->evdp, SHD->mag );
  printf("(read_sac) the ninth row is: %f, %f, %f, %f, %f.\n",SHD->user0, SHD->user1, SHD->user2, SHD->user3, SHD->user4);
  printf("(read_sac) the tenth row is: %f, %f, %f, %f, %f.\n",SHD->user5, SHD->user6, SHD->user7, SHD->user8, SHD->user9 );
  printf("(read_sac) the eleventh row is: %f, %f, %f, %f, %f.\n",SHD->dist, SHD->az, SHD->baz, SHD->gcarc, SHD->internal2);
  printf("(read_sac) the 12th row is: %f, %f, %f, %f, %f.\n",SHD->internal3, SHD->depmen, SHD->cmpaz, SHD->cmpinc, SHD->xminimum );
  printf("(read_sac) the 13th row is: %f, %f, %f, %f, %f.\n",SHD->xmaximum, SHD->yminimum, SHD->ymaximum, SHD->unused1, SHD->unused2);
  printf("(read_sac) the 14th row is: %f, %f, %f, %f, %f.\n",SHD->unused3, SHD->unused4, SHD->unused5, SHD->unused6, SHD->unused7 );
  printf("(read_sac) the 15th row is: %d, %d, %d, %d, %d.\n",SHD->nzyear, SHD->nzjday, SHD->nzhour, SHD->nzmin, SHD->nzsec);
  printf("(read_sac) the 16th row is: %d, %d, %d, %d, %d.\n",SHD->nzmsec, SHD->nvhdr, SHD->norid, SHD->nevid, SHD->npts);
  printf("(read_sac) the 17th row is: %d, %d, %d, %d, %d.\n",SHD->internal4, SHD->nwfid, SHD->nxsize, SHD->nysize, SHD->unused8);
  printf("(read_sac) the 18th row is: %d, %d, %d, %d, %d.\n",SHD->iftype, SHD->idep, SHD->iztype, SHD->unused9, SHD->iinst );
  printf("(read_sac) the 19th row is: %d, %d, %d, %d, %d.\n",SHD->istreg, SHD->ievreg, SHD->ievtyp, SHD->iqual, SHD->isynth);
  printf("(read_sac) the 20th row is: %d, %d, %d, %d, %d.\n",SHD->imagtyp, SHD->imagsrc, SHD->unused10, SHD->unused11, SHD->unused12 );
  printf("(read_sac) the 21st row is: %d, %d, %d, %d, %d.\n",SHD->unused13, SHD->unused14, SHD->unused15, SHD->unused16, SHD->unused17);
  printf("(read_sac) the 22nd row is: %ld, %ld, %ld, %ld, %ld.\n",SHD->leven, SHD->lpspol, SHD->lovrok, SHD->lcalda, SHD->unused18 );
  printf("(read_sac) the 23rd row is: %s, %s.\n",SHD->kstnm, SHD->kevnm);
  printf("(read_sac) the 24th row is: %s, %s, %s.\n",SHD->khole, SHD->ko, SHD->ka );
  printf("(read_sac) the 25th row is: %s, %s, %s.\n",SHD->kt0, SHD->kt1, SHD->kt2);
  printf("(read_sac) the 26th row is: %s, %s, %s.\n",SHD->kt3, SHD->kt4, SHD->kt5);
  printf("(read_sac) the 27th row is: %s, %s, %s.\n",SHD->kt6, SHD->kt7, SHD->kt8);
  printf("(read_sac) the 28th row is: %s, %s, %s.\n",SHD->kt9, SHD->kf, SHD->kuser0);
  printf("(read_sac) the 29th row is: %s, %s, %s.\n",SHD->kuser1, SHD->kuser2, SHD->kcmpnm);
  printf("(read_sac) the 29th row is: %s, %s, %s.\n",SHD->knetwk, SHD->kdatrd, SHD->kinst);
*/
  if ( SHD->npts > nmax ) {
    fprintf(stderr,"ATTENTION !!! %s npts is limited to %d.\n", fname, (int)nmax);
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

