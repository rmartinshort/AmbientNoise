
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../lib/mysac.h"
/*--------------------------------------------------------------------------*/
	void f_fu_trans(int r,float *re,float *im,int isi)
/*--------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
F(O)=int(f(t)*exp(i*2*pi*f*t))
r - la puissance de 2  (npoints = 2**r)

re - real part of the signal (modified on the output)
im - imaginary part of the signal (modified on the output)

fast fourieu transform
isi=1 if inverse and isi=-1 if direct transformation
----------------------------------------------------------------------------*/
{
  int i,j,l,n,lim1,lim2,m,lim3,l1;
  float cs,cstep,sstep,arg,pi2,b1,b2,c1,c2,cs1,si,si1,w;
  float a1,a2;

  pi2=6.2831852;
/*..........................................................................*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
  if (isi != 1 && isi != -1) {
    printf("\n illegal fft parameter !\n");
    exit(1);
  }
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
  /*n=1;	for (i=0;i<r;i++) n=n*2;		n=2**r	*/
  n= 1 << r;

 lim1=n-1;
 lim2=n/2;

 j = 0;

 for (i=0;i<lim1;i++){
/*-----------------------------------------------------------*/
    if (i < j) {
    /*............................*/
      a1=re[j];
      a2=im[j];
      re[j]=re[i];
      im[j]=im[i];
      re[i]=a1;
      im[i]=a2;
    /*............................*/}

    l = lim2;

    for(;;) {
    /*............................*/
      l1=l-1;
      if ( l1 < j )	{ j=j-l; l=l/2; }
      else		break;
    /*............................*/
    }

    j=j+l;
  /*-----------------------------------------------------------*/
  }
  
  lim1=1;
  lim2=n/2;

  for(i=1;i<=r;i++) {
  /*-----------------------------------------------------------*/
    /*l1=i-1;
    lim1=1;	for (j=0;j<l1;j++) lim1=lim1*2;lim1=2**(i-1) */

    /*l1=r-i;
    lim2=1;	for (j=0;j<l1;j++) lim2=lim2*2;lim2=2**(r-i) */

    arg=-pi2*lim2*isi/(float)n;
    cs = 1.0;
    si = 0.0;
    cstep = (float)cos((double)arg);
    sstep = (float)sin((double)arg);

    for (m=1;m<=lim1;m++) {
    /*-------------------------------------------*/
      for (l=1;l<=lim2;l++) {
      /*---------------------------------*/
        lim3 = (m-1)+(l-1)*2*lim1+1;
        b1=re[lim3-1];
        b2=im[lim3-1];
        c1=re[lim3+lim1-1];
        c2=im[lim3+lim1-1];
        a1 = c1*cs+c2*si;
        a2 = -c1*si+c2*cs;
        re[lim3-1]=b1+a1;
        im[lim3-1]=b2+a2;
        re[lim3+lim1-1]=b1-a1;
        im[lim3+lim1-1]=b2-a2;
      /*---------------------------------*/
      }

      cs1 = cs*cstep-si*sstep;
      si1 = si*cstep+cs*sstep;
      cs = cs1;
      si = si1;
    /*-------------------------------------------*/
    }

  lim1=lim1*2;
  lim2=lim2/2;
  /*-----------------------------------------------------------*/
  }

  if (isi == 1)	{
  /*------------------*/
    w=1./(float)n;
    for (i=0;i<n;i++) {
    /*----------------*/
      re[i]=re[i]*w;
      im[i]=im[i]*w;
    /*----------------*/
    }
  /*------------------*/
  }
}



