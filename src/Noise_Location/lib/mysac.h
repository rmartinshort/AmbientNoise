#ifndef MYSAC
#define MYSAC

/* initializing the bool type for logicals */
typedef int boole;
#define TRUE (1)
#define FALSE (2)

/* all the sac header variables to be used in type SAC_HD */
typedef struct sac {                    
float	delta,     depmin,    depmax,    scale,     odelta;       /* 1  */ 
float	b,         e,         o,         a,         internal1;    /* 2  */
float	t0,        t1,        t2,        t3,        t4;           /* 3  */
float	t5,        t6,        t7,        t8,        t9;           /* 4  */
float	f,         resp0,     resp1,     resp2,     resp3;        /* 5  */
float	resp4,     resp5,     resp6,     resp7,     resp8;        /* 6  */
float	resp9,     stla,      stlo,      stel,      stdp;         /* 7  */
float	evla,      evlo,      evel,      evdp,      mag;          /* 8  */
float	user0,     user1,     user2,     user3,     user4;        /* 9  */
float	user5,     user6,     user7,     user8,     user9;        /* 10 */
float	dist,      az,        baz,       gcarc,     internal2;    /* 11 */
float	internal3, depmen,    cmpaz,     cmpinc,    xminimum;     /* 12 */
float	xmaximum,  yminimum,  ymaximum,  unused1,   unused2;      /* 13 */
float	unused3,   unused4,   unused5,   unused6,   unused7;      /* 14 */
int	nzyear,    nzjday,    nzhour,    nzmin,     nzsec;        /* 15 */
int	nzmsec,    nvhdr,     norid,     nevid,     npts;         /* 16 */
int	internal4, nwfid,     nxsize,    nysize,    unused8;      /* 17 */
int	iftype,    idep,      iztype,    unused9,   iinst;        /* 18 */
int	istreg,    ievreg,    ievtyp,    iqual,     isynth;       /* 19 */
int	imagtyp,   imagsrc,   unused10,  unused11,  unused12;     /* 20 */
int	unused13,  unused14,  unused15,  unused16,  unused17;     /* 21 */
boole	leven,     lpspol,    lovrok,    lcalda,    unused18;     /* 22 */
char	kstnm[8],  kevnm[16];                                     /* 23 */
char	khole[8],  ko[8],     ka[8];                              /* 24 */
char	kt0[8],    kt1[8],    kt2[8];                             /* 25 */
char	kt3[8],    kt4[8],    kt5[8];                             /* 26 */
char	kt6[8],    kt7[8],    kt8[8];                             /* 27 */
char	kt9[8],    kf[8],     kuser0[8];                          /* 28 */
char	kuser1[8], kuser2[8], kcmpnm[8];                          /* 29 */
char	knetwk[8], kdatrd[8], kinst[8];                           /* 30 */
} SAC_HD;

//#ifndef MAIN
//extern SAC_HD SAC_HEADER;
//#else
SAC_HD SAC_HEADER;
//#endif

/* initial values for all the sac header variables */
static SAC_HD sac_null = {
-12345., -12345., -12345., -12345., -12345.,
-12345., -12345., -12345., -12345., -12345.,
-12345., -12345., -12345., -12345., -12345.,
-12345., -12345., -12345., -12345., -12345.,
-12345., -12345., -12345., -12345., -12345.,
-12345., -12345., -12345., -12345., -12345.,
-12345., -12345., -12345., -12345., -12345.,
-12345., -12345., -12345., -12345., -12345.,
-12345., -12345., -12345., -12345., -12345.,
-12345., -12345., -12345., -12345., -12345.,
-12345., -12345., -12345., -12345., -12345.,
-12345., -12345., -12345., -12345., -12345.,
-12345., -12345., -12345., -12345., -12345.,
-12345., -12345., -12345., -12345., -12345.,
-12345, -12345, -12345, -12345, -12345,
-12345, -12345, -12345, -12345, -12345,
-12345, -12345, -12345, -12345, -12345,
-12345, -12345, -12345, -12345, -12345,
-12345, -12345, -12345, -12345, -12345,
-12345, -12345, -12345, -12345, -12345,
-12345, -12345, -12345, -12345, -12345,
-12345, -12345, -12345, -12345, -12345,
{ '-','1','2','3','4','5',' ',' ' },
{ '-','1','2','3','4','5',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' },
{ '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
{ '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
{ '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
{ '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
{ '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
{ '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
{ '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
{ '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
{ '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
{ '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
{ '-','1','2','3','4','5',' ',' ' }
};

/* defines for logical data types 
#define TRUE    1
#define FALSE   0 
*/

/* defines for enumerated data types */
/* #define IREAL   0  */  /* not on sac website */
#define ITIME   1 
#define IRLIM   2 
#define IAMPH   3 
#define IXY     4 
#define IUNKN   5 
#define IDISP   6 
#define IVEL    7 
#define IACC    8 
#define IB      9 
#define IDAY   10 
#define IO     11 
#define IA     12 
#define IT0    13 
#define IT1    14 
#define IT2    15 
#define IT3    16 
#define IT4    17 
#define IT5    18 
#define IT6    19 
#define IT7    20 
#define IT8    21 
#define IT9    22 
#define IRADNV 23 
#define ITANNV 24 
#define IRADEV 25 
#define ITANEV 26 
#define INORTH 27 
#define IEAST  28 
#define IHORZA 29 
#define IDOWN  30 
#define IUP    31 
#define ILLLBB 32 
#define IWWSN1 33 
#define IWWSN2 34 
#define IHGLP  35 
#define ISRO   36 
#define INUCL  37 
#define IPREN  38 
#define IPOSTN 39 
#define IQUAKE 40 
#define IPREQ  41 
#define IPOSTQ 42 
#define ICHEM  43 
#define IOTHER 44 
#define IGOOD  45 
#define IGLCH  46 
#define IDROP  47 
#define ILOWSN 48 
#define IRLDTA 49 
#define IVOLTS 50 
#define IXYZ   51
#define IMB    52
#define IMS    53
#define IML    54
#define IMW    55
#define IMD    56
#define IMX    57
#define INEIC  58
#define IPDE   59
#define IISC   60
#define IREB   61
#define IUSGS  62
#define IBRK   63
#define ICALTECH 64
#define ILLNL  65
#define IEVLOC 66
#define IJSOP  67
#define IUSER  68
#define IUNKNOWN 69
#define IQB    70
#define IQB1   71
#define IQB2   72
#define IQBX   73
#define IQMT   74
#define IEQ    75
#define IEQ1   76
#define IEQ2   77
#define IME    78
#define IEX    79
#define INU    80
#define INC    81
#define IO_    82
#define IL     83
#define IR     84
#define IT     85
#define IU     86

#define FCS "%15.7f%15.7f%15.7f%15.7f%15.7f\n"
#define ICS "%10d%10d%10d%10d%10d\n"
#define CCS1 "%-8.8s%-8.8s%-8.8s\n"
#define CCS2 "%-8.8s%-16.16s\n"


	SAC_HD *read_sac (char *fname, float *sig, SAC_HD *SHD, int nmax);
	void write_sac (char *fname, float *sig, SAC_HD *SHD);

#endif 
