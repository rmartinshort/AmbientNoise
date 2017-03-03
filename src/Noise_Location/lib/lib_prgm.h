#ifndef LIB_PRGM_H
#define LIB_PRGM_H

#include "mysac.h"

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif


/* lib_prgm.c */
extern void f_fu_trans(int ,float *,float *,int );

//extern SAC_HD *read_sac (char *, float *, SAC_HD *, int );
//extern void write_sac (char *, float *, SAC_HD *);

#endif
