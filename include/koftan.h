#ifndef KOFTAN
#define KOFTAN

/*--------------------------------------------------------------------
  Certains function utilisent les parameters suivantes.
  Il faut les decrire: pour le faire il faut ajuter la ligne:
#define MAIN
  dans le ficier, avec le text de 'main'
--------------------------------------------------------------------*/
#ifndef MAIN
extern float DIST_, DT_, T0_, ampNORM;
extern int Slen_;
#else
float DIST_, DT_, T0_, ampNORM;
int Slen_;
#endif

#define TEXTSCR screen_simb_atr('°',117)


#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include </usr/include/malloc.h>
#include "gl_const.h"
#include "mymacro.h"
#include "mysac.h"
#define DEL_CURSOR _settextcursor(0x2000)
#define NORM_CURSOR _settextcursor(0x0607)

#define SKD_filt_typ 3
#define Resp_Typ_Disp 1
#define accur .00001

#ifdef __cplusplus
extern "C" {
#endif

/*###################################################################
	random numbers functions
###################################################################*/
 void  m_surand ( double *seed, int *lrn, float *rn);

/*###################################################################
	math. functions
###################################################################*/
	int imin(int i1,int i2);
	int imax(int i1,int i2);
	float fumin(float f1,float f2);
	float fumax(float f1,float f2);
	float fsign(float f) ;
	int isign(float f);
	int nint(float f) ;
	void Regul_2(float *f1,float *f2);
	void Regul2_2 (float *f11,float *f12,float *f21,float *f22);
	void Regul2 (float *f,int n);
	void Regul_XY (float *x,float *y,int n);
	float lin1_interp(float *pointval,float p,int nin);
	float lin_interp(float *pointin,float *valin,float p,int nin);
	float f_lin_interp(float *pointin,float *valin,float p,int nin);
	float lin2 (float p,float p1,float p2,float v1,float v2);
	void f_fu_trans(int r,float *re,float *im,int isi);

/*###################################################################
	SVAN functions
###################################################################*/
 struct svan_par {
	int n2n_s;     /* lenth of input spectrum */
	int nfu;       /* 2 - power for Fourieu transform lenth  */
	int m2b;       /* base lenth for Fourieu transform */
	float dfc;     /* step by frequency */
	int mp;        /* number of used periods (frequencyes) */
	int m1,m2;     /* numbers of first and last central frequency points */
	int npoint[500]; /* numbers of central frequency points */
	int nrat;      /* ratio of output and input time steps ( = 2**n ) */
	int m2m;       /* number of points after F_TRANS */
	int n1;        /* number of first used time point in otput array */
	int nt;        /* number of used time points in otput array */
	float dtsv;    /* output step by time  */
	float beta[500];   /* gaussian window width */
		  };

#ifdef MAIN
	struct svan_par    FT_PAR;
#else
	extern struct svan_par    FT_PAR;
#endif



	struct svan_par* ftan_par(void);
	void setdf (float);
	void inqdf (float *);
	void setdtsv (float);
	void inqdtsv (float *);
	void setmp (int);
	void inqmp(int *);
	void setm1m2 (int,int);
	void inqm1m2 (int *,int *);
	void setnrat (int);
	void inqnrat (int *);
	void setm2m (int);
	void inqm2m (int *);
	void setm2b (int);
	void inqm2b (int *);
	void set_n2n_s (int);
	void inq_nfu (int *);
	void set_nfu (int);
	void inq_n2n_s (int *);
	void setn1nt (int,int);
	void inqn1nt (int *,int *);
	void setnpoint (int *,int);
	void setnpointone (int,int);
	void inqnpointone (int *,int);
	void inqnpoint (int *,int);
	void setbeta (float *,int);
	void setbetaconst (float,int);
	void setalpha (float *,int);
	void setalphaconst (float,int);
	void setalphaone (float,int);
	void setbetaone (float,int);
	void inqbetaone (float *,int);
	void inqbeta (float *,int);

	void groupminmax (float,float);
	void peminmax (float,float);
	void f_set_npointlog (int,int);
	void f_npointlog(void);
	void p_npointlog(void);
	void p_m2m_npointlog(void);
	void set_fconst_point (void);
	void f_ft_trans (float*,float*,float*,float*);
	void windfft (float *,float *,float *,float *,int,float*,float*,int);
	void i_f_ft_trans (float *ar,float *ai,float *amp,float *pha);
	void i_windfft (float *ar,float *ai,float *amp,float *pha,int mptmp,
			float *arw, float *aiw, int n2u);
	void ft_f_trans(float *, float *, float *, float *);
	float g_filter(float, int);
	float exp_c(double);
	void f_tran (float *,float *,int,int,int);

/*#######################################################################
	Special functions for SVAN programm
#######################################################################*/
	float RAYsvanALPHA (float,float);
	float RsALPHA1 (float,float);
	float RsALPHA2 (float,float);
	void cont_beta_set (void);
	void setftanpar (void);
	void Amp_Norm_Log (float *ain,float *aou,float *amax,long n);
	void Amp_Log_Norm (float *ampl,float *ampn,float amax,long n);

/*###################################################################
	Responce functions
###################################################################*/
	float FILT_2 (long n,float dt,float w);
	float FILT_4 (long n,float dt,float *c,float w);
	float FILT_1 (float w1,float w2,float w);
	float SKD_Amp_Resp
	(float Ts,float Tg,float Ds,float Dg,float s2,float w);
	float SKD_Pha_Resp
	(float Ts,float Tg,float Ds,float Dg,float s2,float w);
	float FILTN (long nfilt,long nftyp,float w);
	float FILT (float w);
	void ZER_POL (float *zer,long nz,float *pol,long np,float w,
			float *amp,float *pha);
	float Amp_ZER_POL (float w);
	float Pha_ZER_POL (float w);
	float FLT_Pha (float w);
	float Pha_Resp (float w);
	float Rsp_Amp (float w);
	float Amp_Resp(float w);
	float Amp_Norm_Resp(float w);
	float Norm_Resp_W (float w);
	float Norm_Resp_Max (void);
	void PZ_IRIS_MITPAN
	(float *zer,long nz,float *pol,long np,float camp);
	void P_Z_IRIS_MITP
	(float *zer,long nz,float *pol,long np);
	void Dis_Vel_Acc (char in,char out);
	void Dif_Resp (void);
	void Int_Resp (void);
	void INQ_SET_IRIS_RESP (long nsta,long jchn,long iy,long id);
	void I_S_IRIS_RESP (long nsta,long jchn,long iy,long id);
	void COOR_IRIS_STA (long nsta);
	void Change_dt_npoint (long numbc,long *n,float *point,float *dt);
	void Spe_Pha_Resp_Cor (float *spr,float *spi);

/*###################################################################
	Signal functions
###################################################################*/
	void sei_cos_window(float *ser,float t1,float t2,float t3,float t4);
	void Detrend (float *s);
	void spe_cos_window
		(float *spr,float *spi,float f1,float f2,float f3,float f4);
	void fl_ft_filter
 (float *spr,float *spi,float *crv,long npo,float *t,long unit,long pict);
	void TIME_FILTER (float *spr,float *spi,float *x,long pict);

	void ft_amp_filter (float *amp,float *cup,float *cdown);
	float cos_filter(float x,float x1,float x2,float x3,float x4);
	void reim_ampha (float y, float x, float *a, float *ph);
	void R_I_A_P (float x, float y, float *a, float *ph);
	long N2N2 (long n);
	long Npow (long n);
	float Pha_0_2pi (float ph);

/*###################################################################
	My tools
###################################################################*/
	unsigned int dec2 (char *str);


/*###################################################################
	FILE name-extention
###################################################################*/
	void Get_Ext (char *namin, char *ext);
	void Get_Name (char *namin, char *nam);
	void Set_Ext (char *namin,char *namou,char *ext);
 void Full_Name_Struct (char *namin,char *nam,char *ext,int *len,int *lendot);

	int Str_len (char *str);

	void Up_Reg (char *str);
	void Low_Reg (char *str);

/*###################################################################
	SAC files reading
###################################################################*/
 void R_sac
(char *name,int ln,float *ser,int *N,float *t0,float *dt,float *dist,int nmax);


/*###################################################################
  Herman ph_vel subroutine
###################################################################*/

/*
void ph_vel_ (int *idispl, int *idispr, int *nsph,
  int *mmaxin, float *din, float*ain, float *bin, float *rhoin,
  float *rat, int *iunit, float *qbinv, int *ifunc,
  int *kmax, int *mode, float *ddc, float *sone, int *igr, float *h,
  float *t, int *iq, int *is, int *ie,
  double *c, double *cb);
*/

#ifdef __cplusplus
}
#endif

#endif
