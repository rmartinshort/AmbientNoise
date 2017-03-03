/*
cut_to_sym_component
Program reads in a list of sac files and then averages the positive and negative lag portions and outputs the "symmetric" component

subroutines used
read_sac_d
write_sac_d
km_dist
  degree_dist


*/

/* standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif
#include <float.h>

/* user headers */
#include "mysac.h"

/* global constants */
#define NPTS_SAC_FILE_MAX 20000   //This value is lower than previous declarations because the correlation files are assumed to have less lag time than a full day
#define EARTH_RADIUS 6371
#define PI 3.14159265358979311600
#define TODEG   57.29577950
#define TORAD   (1./TODEG)

typedef struct DISTAZ {
	double distance;
	double degrees;
	double azimuth;
} DISTAZ;



/*-------------subroutines--------------*/
//declarations
extern SAC_HD *read_sac_d (char *, double *, SAC_HD *, int );
extern void write_sac_d (char *, double *, SAC_HD *);
double km_dist(float lat1, float long1, float lat2, float long2);
double degree_dist(float lat1, float long1, float lat2, float long2);
double powi(double b, long x);
DISTAZ distaz(double lat1, double long1, double lat2, double long2);


double powi(double b, long x) {
        double temp=0.0;
        long i;

        if ( b == 0.0 ) {
                return( (double) 0.0 );
        }
        if ( x == 0L ) {
                return( (double) 1.0 );
        }

    if ( x > 0L ) {
                temp = b;
        for ( i = x-1; i > 0L; i-- ) {
                temp *= b;
        }
        return temp;
        }

    if ( x < 0L ) {
        temp = 1.0 / b;
        for ( i = x+1; i < 0L; i++ ) {
                temp *= (1.0/b);
        }
        return temp;
        }
        return temp;
}
///END



DISTAZ distaz(double lat1, double long1, double lat2, double long2)
{
	int laz=1, ldist=1, lxdeg=1;
//	int idx, ns=1;
	double a, a1, a12, a12bot, a12top, al, b, b0, b1, c, c0, c1, c2, 
	 c4, cosa12, costhi, costhk, d, d1, dl, du, e, e1, e1p1, e2, e3, 
	 ec2, el, eps, f, f1, g, g1, h, h1, onemec2, p1, p2, pdist, pherad, 
	 phsrad, sc, sd, sina12, sinthi, sinthk, sqrte1p1, ss, t1, t2, 
	 tanthi, tanthk, temp, therad, thg, thsrad, u1, u1bot, u2, u2bot, 
	 u2top, v1, v2, x2, y2, z1, z2;
	
	double the=lat1, phe=long1, ths=lat2, phs=long2;
	double xdeg, azi, distance;

	DISTAZ out;

	static float rad = 6378.160;  /* Earth Radius */
	static float fl = 0.00335293; /* Earth Flattening */
	static float twopideg = 360.; /* Two Pi in Degrees */
	static float c00 = 1.;
	static float c01 = 0.25;
	static float c02 = -4.6875e-02;
	static float c03 = 1.953125e-02;
	static float c21 = -0.125;
	static float c22 = 3.125e-02;
	static float c23 = -1.46484375e-02;
	static float c42 = -3.90625e-03;
	static float c43 = 2.9296875e-03;
//	static float degtokm = 111.3199;  /* Conversion from degrees to km */


//	double *const Az = &az[0] - 1;
//	double *const Dist = &dist[0] - 1;
//	double *const Phs = &phs - 1;
//	double *const Ths = &ths - 1;
//	double *const Xdeg = &xdeg[0] - 1;


	/* - Initialize. */
	ec2 = 2.*fl - fl*fl;
	onemec2 = 1. - ec2;
	eps = 1. + ec2/onemec2;


	/* - Convert event location to radians.
	 *   (Equations are unstable for latidudes of exactly 0 degrees.) */
	temp = the;
	if( temp == 0. ) {
	    temp = 1.0e-08;
	}
        therad = TORAD*temp;
        pherad = TORAD*phe;
       

	/* - Must convert from geographic to geocentric coordinates in order
	 *   to use the spherical trig equations.  This requires a latitude
	 *   correction given by: 1-EC2=1-2*FL+FL*FL */
	if ( the == 90 || the == -90 ) {/* special attention at the poles */
	    thg = the*TORAD ;		/* ... to avoid division by zero. */
        } else {
            thg = atan( onemec2 * tan(therad) );
        }
	d = sin( pherad );
	e = -cos( pherad );
	f = -cos( thg );
	c = sin( thg );
	a = f*e;
	b = -f*d;
	g = -c*e;
	h = c*d;

	    /* -- Convert to radians. */
	    temp = ths;
	    if( temp == 0. )
		temp = 1.0e-08;
	    thsrad = TORAD*temp;
	    phsrad = TORAD*phs;

	    /* -- Calculate some trig constants. */
	    if ( ths == 90 || ths == -90 )
		thg = ths * TORAD ;
	    else
		thg = atan( onemec2*tan( thsrad ) );
	    d1 = sin( phsrad );
	    e1 = -cos( phsrad );
	    f1 = -cos( thg );
	    c1 = sin( thg );
	    a1 = f1*e1;
	    b1 = -f1*d1;
	    g1 = -c1*e1;
	    h1 = c1*d1;
	    sc = a*a1 + b*b1 + c*c1;

	    /* - Spherical trig relationships used to compute angles. */

	    if( lxdeg ){
		sd = 0.5*sqrt( (powi(a - a1,2) + powi(b - b1,2) + powi(c - 
		 c1,2))*(powi(a + a1,2) + powi(b + b1,2) + powi(c + c1,2)) );
		xdeg = atan2( sd, sc )*TODEG;
		if( xdeg < 0. ) {
		    xdeg = xdeg + twopideg;
		}
	    }

	    if( laz ){
		ss = powi(a1 - d,2) + powi(b1 - e,2) + powi(c1,2) - 2.;
		sc = powi(a1 - g,2) + powi(b1 - h,2) + powi(c1 - f,2) - 2.;
		azi = atan2( ss, sc )*TODEG;
		if( azi < 0. ) {
		    azi = azi + twopideg;
		}
	    }
	    /* - Now compute the distance between the two points using Rudoe's
	     *   formula given in GEODESY, section 2.15(b).
	     *   (There is some numerical problem with the following formulae.
	     *   If the station is in the southern hemisphere and the event in
	     *   in the northern, these equations give the longer, not the
	     *   shorter distance between the two locations.  Since the
	     *   equations are fairly messy, the simplist solution is to reverse
	     *   the meanings of the two locations for this case.) */
	    if( ldist ){
		if( thsrad > 0. ){
		    t1 = thsrad;
		    p1 = phsrad;
		    t2 = therad;
		    p2 = pherad;

		    /* special attention at the poles to avoid atan2 troubles 
		       and division by zero. */
                    if ( the == 90.0 ) {
			costhk = 0.0 ;
			sinthk = 1.0 ;
			tanthk = FLT_MAX ;
		    }
                    else if ( the == -90.0 ) {
			costhk = 0.0 ;
			sinthk = -1.0 ;
			tanthk = -FLT_MAX ;
		    }
                    else {
			costhk = cos( t2 );
			sinthk = sin( t2 );
			tanthk = sinthk/costhk;
                    }

		    /* special attention at the poles continued. */
		    if ( ths == 90.0 ) {
                        costhi = 0.0 ;
                        sinthi = 1.0 ;
                        tanthi = FLT_MAX ;
                    }
		    else if ( ths == -90.0 ) {
                        costhi = 0.0 ;
                        sinthi = -1.0 ;
                        tanthi = -FLT_MAX ;
		    }
		    else {
			costhi = cos( t1 );
			sinthi = sin( t1 );
			tanthi = sinthi/costhi;
		    } 
		}
		else{
		    t1 = therad;
		    p1 = pherad;
		    t2 = thsrad;
		    p2 = phsrad;

		    /* more special attention at the poles */
                    if ( ths == 90.0 ) {
                        costhk = 0.0 ;
                        sinthk = 1.0 ;
                        tanthk = FLT_MAX ;
                    }
                    else if ( ths == -90.0 ) {
                        costhk = 0.0 ;
                        sinthk = -1.0 ;
                        tanthk = -FLT_MAX ;
                    }
		    else {
                        costhk = cos( t2 );
                        sinthk = sin( t2 );
                        tanthk = sinthk/costhk;
                    } 

		    /* more special attention at the poles continued */
                    if ( the == 90.0 ) {
                        costhi = 0.0 ;
                        sinthi = 1.0 ;
                        tanthi = FLT_MAX ;
                    }
                    else if ( the == -90.0 ) {
                        costhi = 0.0 ;
                        sinthi = -1.0 ;
                        tanthi = -FLT_MAX ;
                    }
		    else {
                        costhi = cos( t1 );
                        sinthi = sin( t1 );
                        tanthi = sinthi/costhi;
                    }

		}

		el = ec2/onemec2;
		e1 = 1. + el;
		al = tanthi/(e1*tanthk) + ec2*sqrt( (e1 + powi(tanthi,2))/
		 (e1 + powi(tanthk,2)) );
		dl = p1 - p2;
		a12top = sin( dl );
		a12bot = (al - cos( dl ))*sinthk;

		/* Rewrote these three lines with help from trig identities.  maf 990415 */
		a12 = atan2( a12top, a12bot );
		cosa12 = cos( a12 );
		sina12 = sin( a12 );

		/*cosa12 = sqrt ( a12bot*a12bot / ( a12bot*a12bot + a12top*a12top ) ) ;
		sina12 = sqrt ( a12top*a12top / ( a12bot*a12bot + a12top*a12top ) ) ; */

		e1 = el*(powi(costhk*cosa12,2) + powi(sinthk,2));
		e2 = e1*e1;
		e3 = e1*e2;
		c0 = c00 + c01*e1 + c02*e2 + c03*e3;
		c2 = c21*e1 + c22*e2 + c23*e3;
		c4 = c42*e2 + c43*e3;
		v1 = rad/sqrt( 1. - ec2*powi(sinthk,2) );
		v2 = rad/sqrt( 1. - ec2*powi(sinthi,2) );
		z1 = v1*(1. - ec2)*sinthk;
		z2 = v2*(1. - ec2)*sinthi;
		x2 = v2*costhi*cos( dl );
		y2 = v2*costhi*sin( dl );
		e1p1 = e1 + 1.;
		sqrte1p1 = sqrt( e1p1 );
		u1bot = sqrte1p1*cosa12;
		u1 = atan2( tanthk, u1bot );
		u2top = v1*sinthk + e1p1*(z2 - z1);
		u2bot = sqrte1p1*(x2*cosa12 - y2*sinthk*sina12);
		u2 = atan2( u2top, u2bot );
		b0 = v1*sqrt( 1. + el*powi(costhk*cosa12,2) )/e1p1;
		du = u2 - u1;
		pdist = b0*(c2*(sin( 2.*u2 ) - sin( 2.*u1 )) + c4*(sin( 4.*
		 u2 ) - sin( 4.*u1 )));
		distance = fabs( b0*c0*du + pdist );
	    } /* end if ( ldist ) */

	out.distance = distance;
	out.azimuth = azi;
	out.degrees = xdeg;

	return out;
}



//compute the distance in degrees (requires global constant PI)
double degree_dist(float lat1, float long1, float lat2, float long2) {
        return acos(sin(lat1 * PI / 180.0) * sin(lat2 * PI / 180.0) + cos(lat1 *PI / 180.0) *cos(lat2 * PI / 180.0) * cos((long2 - long1) * PI / 180.0)) * 180.0 / PI;
}
//END


//compute the distance in kilometers (using the EARTH_RADIUS global constant and the degree_dist routine)
double km_dist(float lat1, float long1, float lat2, float long2) {
        return degree_dist(lat1,long1,lat2,long2) * PI / 180.0 * EARTH_RADIUS;
}
//END

//read_sac_d
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

        if ( !SHD ) SHD = &SAC_HEADER;

         fread(SHD,sizeof(SAC_HD),1,fsac);

         if ( SHD->npts > nmax )
         {
          fprintf(stderr,
           "ATTENTION !!! in the file %s the number of points is limited to %d\n",fname,nmax);

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

////write_sac_d
///just like the write_sac above, but with a cast to allow inputting double precision arrays
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

        if ( !SHD ) SHD = &SAC_HEADER;
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
         fwrite(sig,sizeof(double),(int)(SHD->npts),fsac);

        fclose(fsac);
	free(sig);
}
//END





/*--------------MAIN-------------------*/
int main(int argc, char *argv[])
{
	/* local variables */
	int lag, i, count_cut=0, count_total=0;
	char list_file_name[100], buff[100], sac_file_name[100], out_file_name[100];
	double *signal, *out_signal;
	FILE *list_file;
	SAC_HD sac_header;
	DISTAZ distance_and_azimuth;

	/* check usage */
	if (argc != 2) {
		fprintf(stderr,"Usage: %s list_of_sac_files_to_cut\n",argv[0]);
		exit(1);
	}

	/* copy command line arguments */
	sscanf(argv[1],"%s",list_file_name);

	/* open the list file */
	if (! (list_file = fopen(list_file_name,"r"))) {
		fprintf(stderr,"Error: file %s not found!\n",argv[0]);
		exit(2);
	}

	/* allocate memory for the processing */
	signal = malloc(sizeof(signal) * NPTS_SAC_FILE_MAX);
	out_signal = malloc(sizeof(out_signal) * NPTS_SAC_FILE_MAX);

	/* read through the list file which should have a list of sac files */
	while (fgets(buff, 100, list_file)) {
	
	/* increase the total counter */
		count_total++;

	/* copy the buffer string to the sac file name */
		sscanf(buff,"%[A-z,0-9,.,/,_,-]",sac_file_name);

	/* read the sac file */
		if (!read_sac_d(sac_file_name,signal,&(sac_header),NPTS_SAC_FILE_MAX)) {
			fprintf(stderr,"Error: file %s not found\n",sac_file_name);
			fprintf(stderr,"Suggest using the 'ls' command to make the list file\n");
		} else {

	/* increase the found counter */
			count_cut++;

	/* use the header to find the lag time */
			lag = (sac_header.npts - 1) / 2;

	/* add the negative and positive lag */
			out_signal[0] = signal[lag];
			for (i=1; i<lag; i++) {
				out_signal[i] = (signal[lag+i] + signal[lag-i])/2;
			}

	/* update the sac header */
			sac_header.npts = lag+1;
			sac_header.b = 0;
			sac_header.e = lag;
			//sac_header.dist = km_dist(sac_header.stla, sac_header.stlo, sac_header.evla, sac_header.evlo);
			distance_and_azimuth = distaz(sac_header.stla, sac_header.stlo, sac_header.evla, sac_header.evlo);
			sac_header.dist = distance_and_azimuth.distance;
			sac_header.az = distance_and_azimuth.azimuth;
			sac_header.gcarc = distance_and_azimuth.degrees;

	/* make a file name for the new output */
			sprintf(out_file_name,"%s_s",sac_file_name);

	/* write the cut sac file */
			write_sac_d(out_file_name,out_signal,&(sac_header));

	/* done checking if the sac file exist */
		}

	/* let the user know its working */
	if (count_total % 1000 == 0) {
                fprintf(stderr,"Cut %d files.\n",count_cut);
        }

	/* done reading the list file */
	}

	fclose(list_file);
	free(signal);
	free(out_signal);

	/* let the user know its done */	
	fprintf(stderr,"Cut %d out of %d files.\n",count_cut, count_total);

	return 0;
}


